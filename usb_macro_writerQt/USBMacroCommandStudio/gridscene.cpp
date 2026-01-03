#include "gridscene.h"
#include "ui_config.h"
//제미나이가 지랄하길래 한번 진짜 수정 안되나 확인하기 위한 주석
#include <QDebug>




GridScene::GridScene(QObject *parent)
    : QGraphicsScene(parent)
{
    qDebug() << "GridScene 최신파일 적용됨 가장 최근";
    // 씬 기본 크기 (원하면 나중에 조절)
    //setSceneRect(0, 0, 2000, 800);
}

void GridScene::drawBackground(QPainter *painter, const QRectF &rect)
{
    // qDebug() << "drawBackground 호출됨! rect:" << rect;
    // [1] 배경 강제 채우기 (무조건 진한 회색)
    // 스타일시트고 나발이고 무시하고 직접 페인트 붓으로 밀어버립니다.
    painter->fillRect(rect, QColor(230,230,230));

    // [2] 격자 선 설정 (잘 보이는 흰색 실선)
    QPen pen;
    pen.setColor(Qt::white);  // 선 색깔: 흰색
    pen.setWidth(1);          // 선 두께
    pen.setStyle(Qt::SolidLine);
    painter->setPen(pen);

    // [3] 격자 계산 (수학 공식 최적화)


    qreal left   = rect.left();
    qreal right  = rect.right();
    qreal top    = rect.top();
    qreal bottom = rect.bottom();

    // firstLeft: 현재 화면의 왼쪽 끝에서 첫 번째 격자가 시작될 위치
    qreal firstLeft = left - std::fmod(left, GIRD_SIZE);
    qreal firstTop  = top - std::fmod(top, GIRD_SIZE);

    // 세로선 긋기
    for (qreal x = firstLeft; x < right; x += GIRD_SIZE) {
        painter->drawLine(x, top, x, bottom);
    }

    // 가로선 긋기
    for (qreal y = firstTop; y < bottom; y += GIRD_SIZE) {
        painter->drawLine(left, y, right, y);
    }

    // [4] (0,0) 원점 표시 (빨간 십자가)
    // QPen debugPen(Qt::red, 3);
    // painter->setPen(debugPen);
    // painter->drawLine(-50, 0, 50, 0);
    // painter->drawLine(0, -50, 0, 50);
}
