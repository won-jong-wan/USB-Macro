#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QBrush>
#include <QListWidgetItem>
#include <QKeyEvent> // 상단에 추가
#include <QDebug>
#include "won_device_api.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_pushButton_2_clicked();

    void on_pushButton_clicked();

    void on_listWidget_itemDoubleClicked(QListWidgetItem *item);

    void updateSelectedItemsType(char typeChar);

    void on_pushButton_3_clicked();


protected:
    void keyPressEvent(QKeyEvent *event) override; // 키보드 이벤트 재정의

private:
    Ui::MainWindow *ui;
    int item_num;
};
#endif // MAINWINDOW_H
