#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "commandfilemanager.h"

#include <QGraphicsRectItem>
#include <QFileDialog>
#include <QScrollBar>
#include "QString"
#include "QDebug"

#include "ui_config.h"

#define MAX_DOWNLOAD_COUNT 500

/* to do list
 * 명령어들을 스프레드 시트나 별도의 타입으로 저장하는 함수 제공
 *
 *
 * ****** finished ******
 * type 변경 시 표시
 * upload 구현
 * line num, type, cmd 스크롤바 동기화
 * 그래픽 노드 시각화
 * 아이템 옮길 때 타입 동기화 시키기
 * set type  버튼 추가
 * download 구현
 */



MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // line num, type, cmd 스크롤바 동기화
    auto cmdEdit  = ui->listWidget;   // 메인 편집창
    auto lineEdit = ui->numListWidget_2;  // 줄 번호
    auto typeEdit = ui->typeListWidge;  // 타입 표시

    QScrollBar *vbar = cmdEdit->verticalScrollBar();

    connect(vbar, &QScrollBar::valueChanged,
            lineEdit->verticalScrollBar(), &QScrollBar::setValue);
    connect(vbar, &QScrollBar::valueChanged,
            typeEdit->verticalScrollBar(), &QScrollBar::setValue);

    // 버튼 색 레이아웃 설정
    ui->setSbtn->setStyleSheet("color:" FG_COLOR_S ";"
                               "background-color: #E8F2FB;"
                               "font-weight: bold;");
    ui->setCbtn->setStyleSheet("color:" FG_COLOR_C ";"
                               "background-color: #E9F7EE;"
                               "font-weight: bold;");
    ui->setDbtn->setStyleSheet("color:" FG_COLOR_D ";"
                               "background-color: #FFF3E5;"
                               "font-weight: bold;");

    // listWidget 아이템이 드래그로 이동될 때 호출
    connect(ui->listWidget->model(), &QAbstractItemModel::rowsMoved,
            this, &MainWindow::onListRowsMoved);


    m_flowScene = new GridScene();

    init_view();

    item_num = 0;


    int ret = WonDeviceApi::getInstance()->openDevice();
    if (ret < 0){
        Dialog popup;       // 1. 객체 생성 (클래스 이름이 ErrorPopup일 경우)
        popup.exec();       // 2. 창 띄우기 (이 창을 닫을 때까지 뒤에 창 클릭 불가)
    }

    m_manager = new NodeManager(m_flowScene, this);

    // ==========================================
    // [테스트 시나리오]
    // 목표: S는 옆으로, C는 밑으로 가는지 확인
    // ==========================================

    // 1. 시작 (S) -> (0,0) 위치 예상
    m_manager->addCommand(NodeType::Type_S, "S Node");
    m_manager->addCommand(NodeType::Type_C, "C Node");
    m_manager->addCommand(NodeType::Type_Delay, "Delay Node");
    m_manager->addCommand(NodeType::Type_S, "S Node");

}





MainWindow::~MainWindow()
{
    delete m_flowScene;
    delete ui;
}


void MainWindow::init_view(){
    int h = ui->flowView->height();
    int w = ui->flowView->width();

    m_flowScene->setSceneRect(0, 0, w, h);
    // 테스트용 박스
    ui->flowView->setScene(m_flowScene);
    ui->flowView->centerOn(0, 0);
    ui->flowView->setAlignment(Qt::AlignLeft | Qt::AlignTop);
}


void MainWindow::update_view()
{
    if (!m_manager) return;

    // 1) 모든 노드/엣지 삭제
    m_manager->clear();

    // 2) listWidget에 있는 명령들을 순서대로 다시 생성
    for (int i = 0; i < ui->listWidget->count(); ++i)
    {
        QListWidgetItem* item = ui->listWidget->item(i);
        if (!item) continue;

        // 타입 (UserRole에 저장된 char)
        char typeChar = (char)item->data(Qt::UserRole).toInt();
        NodeType type;

        if (typeChar == 'S')      type = NodeType::Type_S;
        else if (typeChar == 'C') type = NodeType::Type_C;
        else if (typeChar == 'D') type = NodeType::Type_Delay;
        else                      type = NodeType::Type_S; // fallback

        // 텍스트
        QString text = item->text();

        // NodeManager에 추가
        m_manager->addCommand(type, text);
    }

    qDebug() << "[update_view] view updated. Node count =" << ui->listWidget->count();
}

void MainWindow::appendCommandRow(char typeChar, const QString &text)
{
    // 현재 개수 기준
    int curCount = ui->listWidget->count();

    // 1) 타입 리스트 (S/C/D)
    QListWidgetItem *typeItem = new QListWidgetItem(QString(typeChar));
    QFont font = typeItem->font();
    typeItem->setTextAlignment(Qt::AlignCenter);
    font.setBold(true);
    font.setPointSize(12);
    typeItem->setFont(font);


    if (typeChar == 'S')
        typeItem->setForeground(QBrush(QColor(FG_COLOR_S)));
    else if (typeChar == 'C')
        typeItem->setForeground(QBrush(QColor(FG_COLOR_C)));
    else if (typeChar == 'D')
        typeItem->setForeground(QBrush(QColor(FG_COLOR_D)));

    ui->typeListWidge->addItem(typeItem);

    // 2) 커맨드 리스트
    QListWidgetItem *cmdItem = new QListWidgetItem(text);
    cmdItem->setData(Qt::UserRole, typeChar);
    cmdItem->setFlags(cmdItem->flags() | Qt::ItemIsEditable);
    cmdItem->setBackground(QColor(220, 220, 220));
    ui->listWidget->addItem(cmdItem);

    // 3) 줄 번호 리스트
    int lineNo = curCount + 1;
    QListWidgetItem *numItem = new QListWidgetItem(QString::number(lineNo));
    numItem->setTextAlignment(Qt::AlignCenter);
    ui->numListWidget_2->addItem(numItem);

    // item_num 동기화
    item_num = ui->listWidget->count();
}

void MainWindow::on_pushButton_2_clicked()
{
    appendCommandRow('S', "");
    update_view();
}

//커맨드 delete하는 함수
void MainWindow::on_pushButton_clicked()
{


    item_num = ui->listWidget->count();
    if(item_num > 0){
        auto list = ui->listWidget->selectedItems();

        if(list.empty()){

            delete ui->listWidget->takeItem(--item_num);
            delete ui->typeListWidge->takeItem(item_num);
            delete ui->numListWidget_2->takeItem(item_num);
        }
        else{
            for(auto item : list){

                int row = ui->listWidget->row(item);
                delete ui->listWidget->takeItem(row);
                delete ui->typeListWidge->takeItem(row);
                // 해당하는 아이템의 인덱스와 일치하는 type item도 지워주어야 함

                delete ui->numListWidget_2->takeItem(item_num-1);
                item_num--;
            }
        }
    }
    update_view();
}



// 더블 클릭 시 복수 선택 해제
void MainWindow::on_listWidget_itemDoubleClicked(QListWidgetItem *item)
{
    ui->listWidget->clearSelection();

}

void MainWindow::updateSelectedItemsType(char typeChar)
{
    QList<QListWidgetItem*> selectedItems = ui->listWidget->selectedItems();
    if (selectedItems.isEmpty()) {
        qDebug() << "선택된 아이템 없음";
        return;
    }
    qDebug() << "선택된 항목 개수:" << selectedItems.size() << "| 설정 타입:" << typeChar;

    for (QListWidgetItem* item : selectedItems) {
        // 1. 아이템 데이터 업데이트 ('S' 또는 'C')
        int row = ui->listWidget->row(item);
        item->setData(Qt::UserRole, typeChar);
        ui->typeListWidge->item(row)->setText(QString(typeChar));
        if (typeChar == 'S')        ui->typeListWidge->item(row)->setForeground(QBrush(QColor(FG_COLOR_S)));
        else if(typeChar == 'C')    ui->typeListWidge->item(row)->setForeground(QBrush(QColor(FG_COLOR_C)));
        else if(typeChar == 'D')    ui->typeListWidge->item(row)->setForeground(QBrush(QColor(FG_COLOR_D)));

    }
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{

    // 1. Delete 키를 눌렀을 때 (선택된 항목 삭제)

    if (event->key() == Qt::Key_Delete) {
        on_pushButton_clicked(); // 기존에 만든 삭제 버튼 함수 호출
    }


    // 2. Enter(Return) 키를 눌렀을 때 (항목 추가)
    else if (event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter) {
        on_pushButton_2_clicked(); // 기존에 만든 추가 버튼 함수 호출
    }


    // set type S (대문자 S 키)
    if (event->key() == Qt::Key_S && (event->modifiers() & Qt::ShiftModifier)) {
        qDebug() << "key event s";
        updateSelectedItemsType('S');
    }
    // set type C (대문자 C 키)
    else if (event->key() == Qt::Key_C && (event->modifiers() & Qt::ShiftModifier)) { // Key_S에서 Key_C로 수정
        qDebug() << "key event c";
        updateSelectedItemsType('C');
    }
    else if (event->key() == Qt::Key_D && (event->modifiers() & Qt::ShiftModifier)) { // Key_S에서 Key_C로 수정
        qDebug() << "key event d";
        updateSelectedItemsType('D');
    }
    // 4. 나머지는 기본 이벤트 처리로 넘김
    else {
        QMainWindow::keyPressEvent(event);
    }

}

/*  upload btn  */
void MainWindow::on_pushButton_3_clicked()
{
    qDebug() << "upload btn clicked!!\n";
    QStringList result;

    for (int i = 0; i < ui->listWidget->count(); ++i) {
        qDebug() << i + 1 << "  " << ui->listWidget->item(i)->text();
        qDebug() << i + 1 << "  "
                 << (char)ui->listWidget->item(i)->data(Qt::UserRole).toInt()
                 << "\n";

        char type   = (char)ui->listWidget->item(i)->data(Qt::UserRole).toInt();
        auto s_cmd  = ui->listWidget->item(i)->text();

        datapacket packet = createPacket(
            2,
            type,
            i == 0,
            i == (ui->listWidget->count() - 1),
            s_cmd.toStdString()
            );

        int ret = WonDeviceApi::getInstance()->writePacket(packet);
        if (ret == -1) {
            qDebug() << "write err!!\n==============packet count :"
                     << i + 1 << "===============";
            printPacket(packet);
            break;
        }

        qDebug() << "write packet sucessful, count: " << i + 1;
    }
}

/*  upload btn     */
void MainWindow::on_downloadBtn_clicked()
{
    qDebug() << "download btn clicked!!\n";

    // 기존 리스트 싹 비우고 시작
    ui->listWidget->clear();
    ui->typeListWidge->clear();
    ui->numListWidget_2->clear();
    item_num = 0;

    datapacket packet;
    uint packet_cnt = 0;

    while (1) {
        packet_cnt++;
        qDebug() << "try packet download...\t (number " << packet_cnt << ")";

        int ret = WonDeviceApi::getInstance()->readPacket(packet);
        if (ret == -1) {
            qDebug() << "[ERROR] Failed to read packet from device.";
            qDebug() << "stop download..";
            printPacket(packet);
            break;
        }

        printPacket(packet);

        // ====== 여기서 packet → typeChar / command 문자열로 파싱 ======
        // ★ struct 정의에 맞게 이 부분은 네 코드 기준으로 맞춰라.
        //   아래는 예시 (type, cmd 필드 있다고 가정):

        char typeChar = ((packet.info >> 3) & 1) ? 'D' : ((packet.info) & 1) ? 'S' : 'C';
        // 예: 'S', 'C', 'D'
                  // 예: null-terminated 문자열

        // packet 내려받은 뒤
        uint16_t len = packet.cmd_len;

        // 안전장치 (패킷 깨졌을 때 대비)
        if (len > sizeof(packet.command))
            len = sizeof(packet.command);

        QString cmd = QString::fromUtf8(packet.command, len);


        appendCommandRow(typeChar, cmd);
        // ============================================================

        // END 플래그(마지막 패킷) 검사 (네가 이미 쓰던 로직 재사용)
        if ((packet.info >> 1) & 1) {
            qDebug() << "[INFO] Download successful.";
            break;
        }

        if (packet_cnt > MAX_DOWNLOAD_COUNT) {
            qDebug() << "[ERROR] Packet count limit exceeded. Stopping download.. (count = "
                     << MAX_DOWNLOAD_COUNT << "X)";
            break;
        }
    }

    // 노드 뷰도 listWidget 기준으로 다시 그리기
    update_view();
}



void MainWindow::on_listWidget_itemChanged(QListWidgetItem *item)
{
    update_view();
}


void MainWindow::onListRowsMoved(const QModelIndex &parent,
                                 int start, int end,
                                 const QModelIndex &destination, int row)
{
    Q_UNUSED(parent);
    Q_UNUSED(start);
    Q_UNUSED(end);
    Q_UNUSED(destination);
    Q_UNUSED(row);

    // 1) 타입/라인넘버 리스트 다시 생성
    ui->typeListWidge->clear();


    for (int i = 0; i < ui->listWidget->count(); ++i) {
        QListWidgetItem *cmdItem = ui->listWidget->item(i);
        if (!cmdItem) continue;

        // ---- 타입 리스트 재생성 ----
        char typeChar = (char)cmdItem->data(Qt::UserRole).toInt();
        QListWidgetItem *typeItem = new QListWidgetItem(QString(typeChar));
        QFont font = typeItem->font();
        font.setBold(true);
        font.setPointSize(12);
        typeItem->setFont(font);
        typeItem->setTextAlignment(Qt::AlignCenter);

        if (typeChar == 'S')
            typeItem->setForeground(QBrush(QColor(FG_COLOR_S)));
        else if (typeChar == 'C')
            typeItem->setForeground(QBrush(QColor(FG_COLOR_C)));
        else if (typeChar == 'D')
            typeItem->setForeground(QBrush(QColor(FG_COLOR_D)));

        ui->typeListWidge->addItem(typeItem);


    }



    // 2) 그래픽 노드도 순서대로 다시 그리기
    update_view();
}



void MainWindow::on_setSbtn_clicked()
{
    updateSelectedItemsType('S');
}


void MainWindow::on_setCbtn_clicked()
{
    updateSelectedItemsType('C');
}


void MainWindow::on_setDbtn_clicked()
{
    updateSelectedItemsType('D');
}


void MainWindow::on_saveBtn_clicked()
{
    QString path = QFileDialog::getSaveFileName(
                                this,
                                "Save Commands",
                                "commands_example.wcmd",
                                "Command Files (*.wcmd)");

    if (path.isEmpty()) return;

    QList<CommandRecord> list;

    // UI → list 변환
    for (int i = 0; i < ui->listWidget->count(); ++i) {
        auto item = ui->listWidget->item(i);

        CommandRecord r;
        r.type = (char)item->data(Qt::UserRole).toInt();
        r.cmd  = item->text();
        list.append(r);
    }

    CommandFileManager::saveToFile(path, list);
}


void MainWindow::on_loadBtn_clicked()
{
    QString path = QFileDialog::getOpenFileName(
        this, "Load Commands", "", "Command Files (*.wcmd)");

    if (path.isEmpty()) return;

    QList<CommandRecord> list;

    if (!CommandFileManager::loadFromFile(path, list))
        return;

    // UI 초기화
    ui->listWidget->clear();
    ui->typeListWidge->clear();
    ui->numListWidget_2->clear();
    item_num = 0;

    // list → UI
    for (auto &rec : list)
        appendCommandRow(rec.type, rec.cmd);

    update_view();
}

