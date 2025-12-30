#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "QString"
#include "QDebug"

#define MAX_DOWNLOAD_COUNT 500
/* to do list
 * type 변경 시 시각화
 *
 *
 *
 *
 * ****** finished ******
 *
 *
 *
 *
 *
 */
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->listWidget->installEventFilter(this);
    item_num = 0;

    int ret = WonDeviceApi::getInstance()->openDevice();
    if (ret < 0){
        Dialog popup;   // 1. 객체 생성 (클래스 이름이 ErrorPopup일 경우)

        popup.exec();       // 2. 창 띄우기 (이 창을 닫을 때까지 뒤에 창 클릭 불가)
    }

}

MainWindow::~MainWindow()
{
    delete ui;
}


//add btn
void MainWindow::on_pushButton_2_clicked()
{


    QChar myChar = 'S';

    // type 표시 하기 위한 typeListWidget에 넣어주기
    QListWidgetItem *tmp_item = new QListWidgetItem("S");
    QFont font = tmp_item->font(); // 현재 폰트 가져오기
    tmp_item->setTextAlignment(Qt::AlignCenter);
    font.setBold(true);        // 굵게
    font.setPointSize(12);
    tmp_item->setFont(font);       // 설정한 폰트 적용

    tmp_item->setForeground(QBrush(QColor("#FF5500")));
    ui->typeListWidge->addItem(tmp_item);


    // commend line 추가
    ui->listWidget->addItem("");

    item_num++;

    ui->listWidget->item(item_num-1)->setData(Qt::UserRole, myChar);    // 두 번째 칸에 char 저장


    auto item = ui->listWidget->item(item_num-1);
    item->setFlags(item->flags()|Qt::ItemIsEditable);
    item->setBackground(QColor(220, 220, 220));

    ui->numListWidget_2->addItem(QString::number(item_num));
    auto numitem =  ui->numListWidget_2->item(item_num-1);
    numitem->setTextAlignment(Qt::AlignCenter);




    //ui->listWidget->clearSelection();  선택 모두 취소
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
        if (typeChar == 'S')        ui->typeListWidge->item(row)->setForeground(QBrush(QColor("#FF5500")));
        else if(typeChar == 'C')    ui->typeListWidge->item(row)->setForeground(QBrush(QColor("#0055FF")));
        else if(typeChar == 'D')    ui->typeListWidge->item(row)->setForeground(QBrush(QColor("#555555")));
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



/*  upload btn     */
void MainWindow::on_pushButton_3_clicked()
{


    qDebug() << "upload btn clicked!!\n";
    QStringList result;

    for(int i = 0; i < ui->listWidget->count(); ++i){
        qDebug() << i + 1<<   "  "<<  ui->listWidget->item(i)->text();
        qDebug() << i + 1<<   "  "<<  (char)ui->listWidget->item(i)->data(Qt::UserRole).toInt() << "\n";


        char type = (char)ui->listWidget->item(i)->data(Qt::UserRole).toInt();
        auto s_cmd = ui->listWidget->item(i)->text();

        datapacket packet = createPacket(2,type, i == 0,  i == (ui->listWidget->count()-1), s_cmd.toStdString());


        int ret = WonDeviceApi::getInstance()->writePacket(packet);
        if (ret == -1){
             qDebug() << "write err!!\n==============packet count :"<< i+1 << "===============";
             printPacket(packet);
             break;
        }

        qDebug() << "write packet sucessful, count: " << i + 1;
    }
}











void MainWindow::on_downloadBtn_clicked()
{

    qDebug() << "download btn clicked!!\n";
    datapacket packet;
    uint packet_cnt = 0;
    while (1){
        packet_cnt++;
        qDebug() << "try packet download...\t (numner " << packet_cnt << ")";

        int ret = WonDeviceApi::getInstance()->readPacket(packet);
        if (ret == -1){
            qDebug() << "[ERROR] Failed to read packet from device.";
            qDebug() << "stop download..";
            printPacket(packet);
            break;
        }
        printPacket(packet);

        if((packet.info >> 2) & 1){
            qDebug() << "[INFO] Download successful.";
            break;
        }
        if (packet_cnt > MAX_DOWNLOAD_COUNT){
            qDebug() << "[ERROR] Packet count limit exceeded. Stopping download.. (count = " << MAX_DOWNLOAD_COUNT << "X)";
            break;
        }
    }

}

