#ifndef COMMAND_FILE_MANAGER_H
#define COMMAND_FILE_MANAGER_H

#include <QString>
#include <QList>

// 개별 명령 한 줄 (타입 + 텍스트)
struct CommandRecord
{
    char    type;   // 'S', 'C', 'D' ...
    QString cmd;    // 실제 명령 문자열
};

// 파일 저장/로드 전용 유틸 – UI와는 독립
class CommandFileManager
{
public:
    // 텍스트/바이너리 포맷은 cpp에서 구현
    // path: 파일 경로
    // commands: 저장할 명령 리스트
    static bool saveToFile(const QString &path,
                           const QList<CommandRecord> &commands);

    // path에서 읽어서 commands에 채움 (기존 내용 clear하고 채우는 쪽으로 구현 추천)
    static bool loadFromFile(const QString &path,
                             QList<CommandRecord> &commands);
};

#endif // COMMAND_FILE_MANAGER_H
