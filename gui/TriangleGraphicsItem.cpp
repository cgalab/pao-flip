/*
 * Copyright (c) 2015 Peter Palfrader
 *
 * All Rights reserved.
 */

#include <CGAL/Bbox_2.h>
#include <CGAL/bounding_box.h>
#include <CGAL/Qt/Converter.h>
#include <QWidget>

#include <string>
#include <iterator>

#include "TriangleGraphicsItem.h"
#include "cgTypes.h"
#include "Tri.h"

TriangleGraphicsItem::TriangleGraphicsItem(Tri * const tri)
	: Base()
	, tri(tri)
	, painterostream(0)
	, vertices_pen(QPen(::Qt::green, 3))
	, segments_pen(QPen(::Qt::blue, 0, Qt::DashLine, Qt::RoundCap, Qt::RoundJoin))
	, labels_pen(QPen(Qt::black, 0, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin)) {

	modelChanged();
	setZValue(3);
}

void
TriangleGraphicsItem::
paint(QPainter *painter, const QStyleOptionGraphicsItem * /*option*/, QWidget * /*widget*/) {
	CGAL::Qt::Converter<K> convert;

	painterostream = CGAL::Qt::PainterOstream<K> (painter);

	if(visible_triangles) {
		painter->setPen(segmentsPen());
		if(tri->isTriangulationDone()) {
			for (ul i = 0; i < (ul)tri->getTriangleData()->numberoftriangles; ++i) {
				auto edges = tri->getTriangleEdgesNotOnInput(i);
				for(auto e : edges) {
					painterostream << e;
				}
			}
	//		for (ul i = 0; i < tri->getTriangleData()->numberofedges; ++i) {
	//			Edge e = tri->getEdge(i);
	//			painterostream << e;
	//		}
		}
	}
	painter->setPen(verticesPen());
	auto transform = painter->worldTransform();
	painter->resetTransform();
	//	for (const auto& i : input->vertices()) {
	//		/* Using this results in "points" that are wide rectangles when one zooms too far in,
	//		 * so we draw out own points after manually transforming.
	//		 * //painterostream << i;
	//		 */
	//
	//		QPointF point = transform.map(convert(i.p));
	//		painter->drawPoint(point);
	//	}
	//	if (visible_edge_labels) {
	//		painter->setPen(labelsPen());
	//		QFont font(painter->font());
	//
	//		font.setPointSize(10);
	//		painter->setFont(font);
	//		for (auto e = input->edges().begin(); e != input->edges().end(); ++e) {
	//			const QPointF p(transform.map(convert( CGAL::midpoint(input->get_segment(*e).source(), input->get_segment(*e).target()) )));
	//			std::string t = "e#"+std::to_string(e - input->edges().begin());
	//			painter->drawText(p.x()+4, p.y(), QString::fromStdString(t));
	//		}
	//	}
	//	if (visible_labels) {
	//		painter->setPen(labelsPen());
	//		QFont font(painter->font());
	//
	//		font.setPointSize(8);
	//		painter->setFont(font);
	//		for (const auto& v : input->vertices()) {
	//			const QPointF p(transform.map(convert(v.p)));
	//			std::string t = "v#"+std::to_string(v.id);
	//			painter->drawText(p.x()+4, p.y(), QString::fromStdString(t));
	//		}
	//	}
	painter->setWorldTransform(transform);
}

void
TriangleGraphicsItem::
updateBoundingBox() {
	CGAL::Qt::Converter<K> convert;
	prepareGeometryChange();

	if (tri->isEmpty()) {
		return;
	}

	auto bb = tri->getPoint(0).bbox();
	for (ul i = 0; i < (ul)tri->getTriangleData()->numberofpoints; ++i) {
		bb += tri->getPoint(i).bbox();
	}

	bounding_rect = convert(bb);
}

void
TriangleGraphicsItem::
modelChanged() {
	updateBoundingBox();
}
