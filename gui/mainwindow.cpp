#include "mainwindow.h"

#include "ui_mainwindow.h"

#include "gml/GMLGraph.h"

MainWindow::MainWindow(const std::string& title, Pao& pao) :
	CGAL::Qt::DemosMainWindow(),
	title(title),
	ui(new Ui::MainWindow),
	pao(pao) {

	ui->setupUi(this);
	setWindowTitle(QString::fromStdString(title));
	scene.setItemIndexMethod(QGraphicsScene::NoIndex);
	ui->gV->setScene(&scene);
	ui->gV->setMouseTracking(true);
	ui->gV->scale(1, -1);
	ui->gV->setHorizontalScrollBarPolicy(Qt::ScrollBarPolicy::ScrollBarAlwaysOff);
	ui->gV->setVerticalScrollBarPolicy(Qt::ScrollBarPolicy::ScrollBarAlwaysOff);
	setAcceptDrops(true);

	//addNavigation(ui->gV);

	/* add navigation */
	navigation = new MyGraphicsViewNavigation();
	ui->gV->viewport()->installEventFilter(navigation);
	ui->gV->installEventFilter(navigation);
	QObject::connect(navigation, SIGNAL(mouseCoordinates(QString)),
			xycoord, SLOT(setText(QString)));
	this->view = ui->gV;

	input_gi = std::make_shared<InputGraphicsItem>(&pao.data->getBasicInput());
	scene.addItem(input_gi.get());

	triangle_gi = std::make_shared<TriangleGraphicsItem>(&pao.tri);
	scene.addItem(triangle_gi.get());

	auto input_size = input_gi->boundingRect().size();
	auto size_avg = (input_size.width() + input_size.height() ) /2.0;

	time_label = new QLabel("", this);
	update_time_label();
	time_label->setAlignment(::Qt::AlignHCenter);
	time_label->setMinimumSize(time_label->sizeHint());
	ui->statusBar->addWidget(new QLabel("C-S-<right button: center; C-<right button>: drag; C-<left buttom>: select-zoom"), 1);
	ui->statusBar->addWidget(new QLabel(this), 1);
	ui->statusBar->addWidget(time_label, 0);
	ui->statusBar->addWidget(xycoord, 0);

	time_changed();
	on_actionResize_triggered();
}

MainWindow::~MainWindow() {}

void MainWindow::updateVisibilities() {
	input_gi->setVisibleLabels(ui->actionVisToggleInputLabels->isChecked());
	input_gi->setVisibleEdgeLabels(ui->actionVisToggleInputEdgesLabels->isChecked());
	input_gi->setVisible(ui->actionVisToggleInput->isChecked());

	triangle_gi->setVisibleTriangles(ui->actionVisToggleTriangle->isChecked());


//	scene.removeItem(skeleton_gi.get());
//	scene.addItem(skeleton_gi.get());
//	skeleton_gi->setVisibleLabels(ui->actionVisToggleInputLabels->isChecked());
//	skeleton_gi->setVisible(ui->actionVisToggleInput->isChecked());
//    skeleton_gi->setVisible(ui->actionVisToggleArcs->isChecked());
//	skeleton_gi->setVisibleArcLabels(ui->actionVisToggleArcLabels->isChecked());
}

void MainWindow::on_actionToggleFullscreen_triggered() {
	if (this->isFullScreen()) {
		this->showNormal();
	} else {
		this->showFullScreen();
	}
	on_actionResize_triggered();
}

void MainWindow::on_actionResize_triggered() {
	auto br = input_gi->boundingRect();
	//br |= skeleton_gi->boundingRect();

	ui->gV->setSceneRect(br);
	ui->gV->fitInView(br, Qt::KeepAspectRatio);
}

void MainWindow::on_actionResetAll_triggered() {
	//monos.reset();

	on_actionResize_triggered();
}

void MainWindow::showEvent(QShowEvent *) {}

void MainWindow::mousePressEvent(QMouseEvent *event) {
	(void) event;
}

void MainWindow::update_time_label() {
	scene.update(scene.sceneRect());
//	auto t = CGAL::to_double( monos.wf->getTime() );
	auto t = CGAL::to_double( 42 );
	time_label->setText(QString("t: %1 ").arg(t));
}

void MainWindow::time_changed() {
	update_time_label();
}


void MainWindow::on_actionEventStep_triggered() {
	if(!pao.config.isValid()) {return;}

	if(!pao.tri.isFlippingDone()) {
		auto cnt = pao.tri.getFlipCnt();
		while(!pao.tri.isFlippingDone() && cnt == pao.tri.getFlipCnt()) {
			pao.tri.aSingleFlip();
		}

		scene.removeItem(input_gi.get());
		scene.addItem(input_gi.get());

		scene.removeItem(triangle_gi.get());
		scene.addItem(triangle_gi.get());

		if(pao.tri.isFlippingDone()) {
			pao.data->printPermutation();
		}
	}
	time_changed();
}


void MainWindow::simulation_has_finished() {
	if (did_finish) return;
	did_finish = true;
	updateVisibilities();
}

void MainWindow::dragEnterEvent(QDragEnterEvent *e) {
    if (e->mimeData()->hasUrls()) {
        e->acceptProposedAction();
    }
}

void MainWindow::dropEvent(QDropEvent *e) {
    foreach (const QUrl &url, e->mimeData()->urls()) {
        QString fileName = url.toLocalFile();
        LOG(INFO) << "Dropped file:" << fileName.toStdString();
        if (fileName.endsWith(".obj") || fileName.endsWith(".graphml")) {

        	//pao.reinitialize(fileName.toStdString(),true);

        	input_gi = std::make_shared<InputGraphicsItem>(&pao.data->getBasicInput());
        	scene.addItem(input_gi.get());

        	on_actionResize_triggered();
        }
    }
}
