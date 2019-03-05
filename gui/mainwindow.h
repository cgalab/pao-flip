#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <CGAL/Qt/DemosMainWindow.h>
#include <QGraphicsScene>
#include <QLabel>

#include <QMimeData>

#include "InputGraphicsItem.h"
#include "TriangleGraphicsItem.h"
#include "Config.h"
#include "Pao.h"

#include "ui_mainwindow.h"

class MainWindow : public CGAL::Qt::DemosMainWindow {
    Q_OBJECT

  public:
    explicit MainWindow(const std::string& title, Pao& pao);
    ~MainWindow();
  private:
    bool first_show_event = true;
    bool did_finish = false;

  private slots:
    void showEvent(QShowEvent *);
    void mousePressEvent(QMouseEvent *event);

    void on_actionQuit_triggered() { close(); };
    void on_actionVisToggleInput_triggered() { updateVisibilities(); };
    void on_actionVisToggleInputLabels_triggered() { updateVisibilities(); };
    void on_actionVisToggleInputEdgesLabels_triggered() { updateVisibilities(); }
    void on_actionVisToggleTriangle_triggered() {updateVisibilities();}
	void on_actionResize_triggered();
    void on_actionToggleFullscreen_triggered();

    void on_actionEventStep_triggered();
    void on_actionResetAll_triggered();

    void dragEnterEvent(QDragEnterEvent *e);
    void dropEvent(QDropEvent *e);

  private:
    std::string title;
    std::unique_ptr<Ui::MainWindow> ui;
    QGraphicsScene scene;
    QLabel* time_label;
    Exact drawing_time_offset_increment;

    Pao& pao;

    std::shared_ptr<InputGraphicsItem> input_gi;
    std::shared_ptr<TriangleGraphicsItem> triangle_gi;

    void updateVisibilities();
    void update_time_label();
    void time_changed();
    void simulation_has_finished();
};


#endif // MAINWINDOW_H
