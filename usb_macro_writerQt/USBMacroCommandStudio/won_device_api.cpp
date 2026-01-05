#include "won_device_api.h"
#include <iomanip>
#include <cstring>

#include <QString>
#include <cstring>
#include <algorithm>
#include <cstdint>
// mainwindow.h

datapacket createPacket(uint8_t version, uint8_t type, bool start, bool isEnd, const std::string& cmd) {
    datapacket packet;
    // 1. 구조체 초기화 (쓰레기 값 제거)
    std::memset(&packet, 0, sizeof(datapacket));

    // 2. Magic Number 설정
    // 필요 시 htonl() 등으로 엔디안 변환 고려 가능
    packet.magic = 0xDEADBEEF;

    // 3. Info 바이트 구성 (비트 필드)
    // [7:4] version, [3] reserved, [2] start, [1] isEnd, [0] type
    uint8_t info = 0;
    info |= (version & 0x0F) << 4;
    info |= ((type == 'D') & 0x01) << 3;       // Reserved
    info |= (start ? 1 : 0) << 2;
    info |= (isEnd ? 1 : 0) << 1;
    info |= ((type == 'S' || type == 'D') & 0x01) << 0;
    packet.info = info;

    // 4. 커맨드 길이 계산 및 안전한 복사
    // 배열 크기(249)를 초과하지 않도록 보장 (마지막 '\0' 자리 확보)
    constexpr size_t MAX_CMD_LEN = sizeof(packet.command) - 1;
    uint16_t actual_len = static_cast<uint16_t>(std::min(cmd.size(), MAX_CMD_LEN));

    packet.cmd_len = actual_len; // 필요 시 htons(actual_len) 고려

    if (actual_len > 0) {
        std::memcpy(packet.command, cmd.data(), actual_len);
    }
    packet.command[actual_len] = '\0'; // 널 종료 문자 보장

    return packet;
}


void printPacket(const datapacket& packet) {
    // 1. 비트 필드 데이터 추출   delay | start | is_end | ttpe
    uint8_t version = (packet.info >> 4) & 0x0F; // 상위 4비트
    bool delay      = (packet.info >> 3) & 0x01; // 4번 비트
    bool start      = (packet.info >> 2) & 0x01; // 3번 비트
    bool isEnd      = (packet.info >> 1) & 0x01; // 2번 비트
    bool type    = (packet.info) & 0x01; // 1번 비트

    // 하위 2비트는 ZERO (예약 영역)

    // 2. 출력 포맷 구성
    qDebug() << "--------------------------------------------";
    qDebug() << "[OUTGOING PACKET]";

    // Magic: 0x를 붙여 8자리 16진수로 출력
    qDebug() << "  - Magic   :" << QString("0x%1").arg(packet.magic, 8, 16, QChar('0')).toUpper();

    // Info 필드 상세
    qDebug() << "  - InfoRaw :" << QString("0x%1 (Binary: %2)")
                                       .arg(packet.info, 2, 16, QChar('0')).toUpper()
                                       .arg(QString::number(packet.info, 2).rightJustified(8, '0'));
    qDebug() << "    └─ Version :" << version;
    qDebug() << "    └─ delay    :" << (delay ? "YES (delay type)" : "NO");
    qDebug() << "    └─ start   :" << (start ? "YES (start Packet)" : "NO");
    qDebug() << "    └─ isEnd   :" << (isEnd ? "YES (Final Packet)" : "NO");
    qDebug() << "    └─ Type    :" << (type ? "S" : "C");
    // Command 정보
    qDebug() << "  - Length  :" << packet.cmd_len << "bytes";

    // Command 내용 (문자열인 경우)
    // QString::fromLocal8Bit는 char배열을 Qt 문자열로 안전하게 변환합니다.
    QString cmdStr = QString::fromLocal8Bit(packet.command, packet.cmd_len);
    qDebug() << "  - Command :" << cmdStr;
    qDebug() << "--------------------------------------------";
}
