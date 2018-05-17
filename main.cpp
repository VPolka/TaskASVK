#include <iostream>
#include <vector>
#include <algorithm>
#include <queue>
#include <string>
#include <cstring>
#include "tinyxml.h"

using namespace std;

// �����, ���������� ��� ���������� � ������ ������
class Job {
public:
    string name = "name";
    int time_start = 0, time_finish = 0, time_dur = 0, num = 0;     /* num - ���������� ����� ������, ����� ��� ������
                                                                    � ���������� ������� */
    Job() {}
};


// �����, ���������� ��� ����������, ������ ��� ������
class Result {
public:
    string name;
    int num, time = 0;
    bool check;                                                     /* ����������, ������������, ����� �� ������
                                                                    �����������, ��� ��� */

    Result(string s, int n, int t = 0, bool ch = false) : name(s), num(n), time(t), check(ch) {}
};

// ���������� ��� ���������� ����� �� ������� ������
bool
cmp_job_sort(const Job &a, const Job &b) {
    return a.time_start < b.time_start;
}

// ���������� ��� ���������� ����� �� �������� �� �����
bool
cmp_res_sort(const Result &a, const Result &b) {
    return a.num < b.num;
}

// ��������� ��� ������� ����� - � ���������� �������� ����������
struct cmp_job_q {
    bool operator() (const Job &a, const Job &b) const {
        return (a.time_dur > b.time_dur);
    }
};

int main()
{
    vector <Result> res;                                // ������, �������� ���������
    vector <Job> v;                                     // ������, �������� ��� ������, ������� ������������ ����� ���� ���������
    priority_queue <Job, vector <Job>, cmp_job_q> q;    /* ������� � ����������� �� ������� ����������, ��������
                                                        ������, ������� ����� ���� ��������� �� ������ ������ */
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

    // ���������� ����� �� ������� ������
    sort(v.begin(), v.end(), cmp_job_sort);
    unsigned int i = 0;

    // ����������� ����������
    while (i < v.size()) {
        // ���� �� ������ ������ ��� �����, ������� ����� �����������, �� ����� ����������� �����
        if (!q.size()) {
            if (t <= v[i].time_start) {
                t = v[i].time_start;
            }
        }
        // ��������� ������ � �������, ���� ��� ����� �����������
        if (v[i].time_start <= t) {
            q.push(v[i]);
            ++i;
            continue;
        }
        // ����� ��������� ������
        if (q.top().time_finish >= t + q.top().time_dur) {
            res.push_back({ q.top().name, q.top().num, t, true });
            t += q.top().time_dur;
        }
        else {
            res.push_back({ q.top().name, q.top().num });
        }
        // ������� �� ������� �������������� ������
        q.pop();
    }

    // �������������� �������
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

    // ���������� ������ � �������� �������
    sort(res.begin(), res.end(), cmp_res_sort);

    // ������� ���� � ���������� � ���� ���������
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
