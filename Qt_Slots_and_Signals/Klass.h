#ifndef _KLASS_H_
#define	_KLASS_H_

#include <QDir>
#include <QObject>
#include <QString>
#include <QTimer>
#include <QFileInfo>
#include <QProcess>
#include <QSocketNotifier>

#include <vector>
#include <string>
#include <set>
#include <utility>
#include <iostream>
#include <cmath>
#include <algorithm>
#include <functional>

class Klass : public QObject {
	Q_OBJECT
private:
	QDir dir;
	QFileInfoList list;
	QSocketNotifier* notifier;
	std::vector<std::pair<int, std::string>> v;
	std::function<bool(const std::string&, const std::string&)> comp;
	std::set<std::string, decltype(comp)> names;
	std::vector<bool> deleted;
	QTimer* timer;
	int size;


public:
	Klass(QObject *p = 0);

private:

	void Fill();

public slots:

	void Update();

signals:
	void Exit();

private slots:

	void Print() ;

	void Remove();

};

#endif // !_KLASS_H_