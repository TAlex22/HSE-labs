#include "Klass.h"

Klass::Klass(QObject *p) : QObject(p) {
	timer = new QTimer(this);
	timer->setInterval(2000);
	notifier = new QSocketNotifier(fileno(stdin), QSocketNotifier::Read, this);
	connect(notifier, SIGNAL(activated(int)), this, SLOT(Update()));
	connect(timer, SIGNAL(timeout()), this, SLOT(Print()));
	connect(timer, SIGNAL(timeout()), this, SLOT(Remove()));
	size = 0;
	comp = [this](const std::string& first, const std::string& second) {
		auto it1 = std::find_if(v.begin(), v.end(), [first](const std::pair<int, std::string>& p) {
			return first == p.second;
			});
		auto it2 = std::find_if(v.begin(), v.end(), [second](const std::pair<int, std::string>& p) {
			return second == p.second;
			});
		return it1->first < it2->first;
	};
	names = std::set<std::string, decltype(comp)>(comp);
	Fill();
	std::cout << "> " << std::flush;
}

void Klass::Fill() {
	list = dir.entryInfoList();
	for (int i = 0; i < list.size(); ++i) {
		if (list[i].fileName() != ".." && list[i].fileName() != ".") {
			size += list[i].size();
			v.push_back({ list[i].size(), list[i].fileName().toStdString() });
		}
	}
	deleted.assign(v.size(), false);
	for (size_t i = 0; i < v.size(); ++i) {
		names.insert(v[i].second);
	}
}

void Klass::Update() {
	std::string command;
	std::getline(std::cin, command);
	if (command == "stop") {
		timer->stop();
		std::cout << "> " << std::flush;
	} else if (command == "start") {
		timer->start();
		std::cout << "> " << std::flush;
	} else if (command == "restart") {
		timer->stop();
		size = 0;
		v.clear();
		names.clear();
		Fill();
		timer->start();
		std::cout << "> " << std::flush;
	} else if (QString str = QString::fromStdString(command); str.left(7) == "timeout") {
		int new_interval = str.split(" ")[1].toInt();
		if (new_interval != 0) {
			timer->setInterval(new_interval);
		}
		std::cout << "> " << std::flush;
	} else if (command == "exit") {
		timer->stop();
		emit Exit();
	} else {
		std::cout << "> " << std::flush;
	}
}

void Klass::Print() {
	std::cout << "{ ";
	size_t i = 0;
	for (const auto& name : names) {
		std::cout << name;
		if (i != names.size() - 1) {
			std::cout << ", ";
		}
		++i;
	}
	std::cout << " }" << std::endl;
}

void Klass::Remove() {
	if (names.size() == 0) {
		timer->stop();
		v.clear();
		Fill();
		timer->start();
		std::cout << "> " << std::flush;
	}
	double avg = (double)size / names.size();
	std::string min_name = v[0].second;
	int sz = 0;
	double min = size + 1;
	size_t idx = 0;
	for (size_t i = 0; i < v.size(); ++i) {
		double val = std::sqrt((v[i].first - 0.75 * avg) * (v[i].first - 0.75 * avg));
			if (val < min && !deleted[i]) {
				min = val;
				min_name = v[i].second;
				sz = v[i].first;
				idx = i;
			}
	}
	deleted[idx] = true;
	size -= sz;
	auto it = names.find(min_name);
	names.erase(it);	
}