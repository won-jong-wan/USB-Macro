#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QBrush>
#include <QListWidgetItem>
#include <QKeyEvent> // 상단에 추가
#include <QDebug>
#include <QGraphicsView>
#include <QGraphicsScene>


#include "won_device_api.h"
#include "dialog.h"
#include "gridscene.h"
#include "nodemanager.h"

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

private:
    void init_view();
    void update_view();
    void appendCommandRow(char typeChar, const QString &text);

private slots:

    void on_pushButton_2_clicked();

    void on_pushButton_clicked();

    void on_listWidget_itemDoubleClicked(QListWidgetItem *item);

    void on_listWidget_itemChanged(QListWidgetItem *item);

    void onListRowsMoved(const QModelIndex &parent,
                         int start, int end,
                         const QModelIndex &destination, int row);

    void updateSelectedItemsType(char typeChar);

    void on_pushButton_3_clicked();

    void on_downloadBtn_clicked();

    void on_setSbtn_clicked();

    void on_setCbtn_clicked();

    void on_setDbtn_clicked();




    void on_saveBtn_clicked();

    void on_loadBtn_clicked();

protected:
    void keyPressEvent(QKeyEvent *event) override; // 키보드 이벤트 재정의

protected:
    // 기존 keyPressEvent 대신 이걸로 처리합니다.
    //bool eventFilter(QObject *obj, QEvent *event) override;

    GridScene *m_flowScene;

    //QGraphicsScene *m_flowScene;
private:
    NodeManager *m_manager;   // 매니저

    Ui::MainWindow *ui;
    int item_num;
};
#endif // MAINWINDOW_H
