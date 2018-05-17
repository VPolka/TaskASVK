#include <iostream>
#include <vector>
#include <algorithm>
#include <queue>
#include <string>
#include <cstring>
#include "tinyxml.h"

using namespace std;

// Класс, содержащий всю информацию о данной работе
class Job {
public:
    string name = "name";
    int time_start = 0, time_finish = 0, time_dur = 0, num = 0;     /* num - порядковый номер работы, нужен для вывода
                                                                    в правильном порядке */
    Job() {}
};


// Класс, содержащий всю информацию, нужную для вывода
class Result {
public:
    string name;
    int num, time = 0;
    bool check;                                                     /* Переменная, обозначающая, будет ли работа
                                                                    выполняться, или нет */

    Result(string s, int n, int t = 0, bool ch = false) : name(s), num(n), time(t), check(ch) {}
};

// Компаратор для сортировки работ по времени начала
bool
cmp_job_sort(const Job &a, const Job &b) {
    return a.time_start < b.time_start;
}

// Компаратор для сортировки работ по порядоку их ввода
bool
cmp_res_sort(const Result &a, const Result &b) {
    return a.num < b.num;
}

// Приоритет для очереди работ - с наименьшим временем выполнения
struct cmp_job_q {
    bool operator() (const Job &a, const Job &b) const {
        return (a.time_dur > b.time_dur);
    }
};

int main()
{
    vector <Result> res;                                // Вектор, хранящий результат
    vector <Job> v;                                     // Вектор, хранящий все работы, которые потенциально могут быть выполнены
    priority_queue <Job, vector <Job>, cmp_job_q> q;    /* Очередь с приорететом по времени выполнения, хранящую
                                                        работы, которые могут быть выполнены на данный момент */
    TiXmlDocument input;
    input.LoadFile("input_static.xml");
    TiXmlElement *p = input.FirstChildElement();
    int cnt = 0;
    for (TiXmlElement *tjob = p->FirstChildElement(); tjob != nullptr; tjob = tjob->NextSiblingElement()) {
        ++cnt;
        Job j;
        j.num = cnt;
        j.name = tjob->Attribute("name");
        j.time_start = atoi(tjob->Attribute("start_deadline"));
        j.time_finish = atoi(tjob->Attribute("finish_deadline"));
        j.time_dur = atoi(tjob->Attribute("duration"));
        if (j.time_finish - j.time_start < j.time_dur) {
            res.push_back({ j.name, j.num });
        }
        else {
            v.push_back(j);
        }
    }
    int t = 0;

    // Сортировка работ по времени старта
    sort(v.begin(), v.end(), cmp_job_sort);
    unsigned int i = 0;

    // Составление расписания
    while (i < v.size()) {
        // Если на данный момент нет работ, которые могут выполняться, то нужно передвинуть время
        if (!q.size()) {
            if (t <= v[i].time_start) {
                t = v[i].time_start;
            }
        }
        // Добавляем работу в очередь, если она может выполняться
        if (v[i].time_start <= t) {
            q.push(v[i]);
            ++i;
            continue;
        }
        // Выбор следующей работы
        if (q.top().time_finish >= t + q.top().time_dur) {
            res.push_back({ q.top().name, q.top().num, t, true });
            t += q.top().time_dur;
        }
        else {
            res.push_back({ q.top().name, q.top().num });
        }
        // Убираем из очереди использованную работу
        q.pop();
    }

    // Дообрабатываем очередь
    while (q.size()) {
        if (q.top().time_finish > t + q.top().time_dur) {
            res.push_back({ q.top().name, q.top().num, t, true });
            t += q.top().time_dur;
        }
        else {
            res.push_back({ q.top().name, q.top().num });
        }
        q.pop();
    }

    // Выставляем работы в исходный порядок
    sort(res.begin(), res.end(), cmp_res_sort);

    // Создаем файл и записываем в него результат
    TiXmlDocument output;
    TiXmlElement *tagtrace = new TiXmlElement("trace");
    output.LinkEndChild(tagtrace);
    for (auto j : res) {
        if (j.check) {
            TiXmlElement *tstart = new TiXmlElement("start");
            tstart->SetAttribute("name", j.name.c_str());
            tstart->SetAttribute("time", to_string(j.time).c_str());
            tagtrace->LinkEndChild(tstart);
        }
        else {
            TiXmlElement *tunsched = new TiXmlElement("unsched");
            tunsched->SetAttribute("name", j.name.c_str());
            tagtrace->LinkEndChild(tunsched);
        }
    }
    output.SaveFile("output_static.xml");
    return 0;
}
