#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "QString"
#include "QDebug"


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


    WonDeviceApi::getInstance()->openDevice();
}

MainWindow::~MainWindow()
{
    delete ui;
}


//add btn
void MainWindow::on_pushButton_2_clicked()
{

    char myChar = 'S';




    QListWidgetItem *tmp_item = new QListWidgetItem("S");
    QFont font = tmp_item->font(); // 현재 폰트 가져오기
    tmp_item->setTextAlignment(Qt::AlignCenter);
    font.setBold(true);        // 굵게
    font.setPointSize(12);

    tmp_item->setFont(font);       // 설정한 폰트 적용

    tmp_item->setForeground(QBrush(QColor("#FF5500")));
    ui->typeListWidge->addItem(tmp_item);


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


void MainWindow::on_pushButton_clicked()
{
    item_num = ui->listWidget->count();
    if(item_num > 0){
        auto list = ui->listWidget->selectedItems();

        if(list.empty()){
            ui->listWidget->takeItem(--item_num);
            ui->numListWidget_2->takeItem(item_num);

        }
        else{
            for(auto item : list){
                ui->listWidget->takeItem(ui->listWidget->row(item));
                ui->numListWidget_2->takeItem(item_num-1);
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
        item->setData(Qt::UserRole, typeChar);

        // 2. 패킷 생성 (이전에 만든 createQtPacket 함수 활용)
        // item->data(Qt::)은 이전에 저장한 typeBit를 가져옵니다.


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
    if (event->key() == Qt::Key_S) {
        qDebug() << "key event s";
        updateSelectedItemsType('S');
    }
    // set type C (대문자 C 키)
    else if (event->key() == Qt::Key_C) { // Key_S에서 Key_C로 수정
        qDebug() << "key event c";
        updateSelectedItemsType('C');
    }

    // 4. 나머지는 기본 이벤트 처리로 넘김
    else {
        QMainWindow::keyPressEvent(event);
    }

}



void MainWindow::on_pushButton_3_clicked()
{

    uint end_idx = 0;;
    uint start_idx;

    qDebug() << "save BTN clicked!!\n";
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

