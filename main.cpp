#include "utils.h"
#include "cm.h"
#include "cu.h"
#include "as.h"
#include "ss.h"
#include "hg.h"
#include "ts.h"
#include "murmur3.h"

#include <string.h>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <map>
#include <cmath>
#include <algorithm>
#include <stdint.h>
#include <iomanip>
using namespace std;

#define ft first
#define sc second

vector<int> flow;
string file_name;

void LoadData_IP(char file[], int flow_size)
{   
    ifstream is(file, ios::in | ios::binary);
    char buf[2000] = {0};

    for (int i = 1; i <= flow_size; i++)
    {
        if(!is.read(buf, 16))
        {
            panic("Data Loading Error.\n");
        }
        int id = MurmurHash3_x86_32((void*)(buf+8), 8, 2022);
        flow.push_back(id);
    }

    cout << "Loading complete. flow_size: " << flow_size << endl;
}

void LoadData_Web(char file[], int flow_size)
{   
    ifstream is(file, ios::in | ios::binary);
    char buf[2000] = {0};

    for (int i = 1; i <= flow_size; i++)
    {
        if(!is.read(buf, 13))
        {
            panic("Data Loading Error.\n");
        }
        int id = MurmurHash3_x86_32((void*)(buf), 4, 2022);
        flow.push_back(id);
    }

    cout << "Loading complete. flow_size: " << flow.size() << endl;
}

void LoadData_Click(char file[], int flow_size)
{   
    ifstream is(file, ios::in | ios::binary);
    char buf[2000] = {0};
    int id;

    for (int i = 1; i <= flow_size; i++)
    {
        if(!(is >> id))
        {
            break;
            // panic("Data Loading Error.\n");
        }
        int id = MurmurHash3_x86_32((void*)&id, 4, 2022);
        flow.push_back(id);
    }

    cout << "Loading complete. flow_size: " << flow.size() << endl;
}

void LoadData_Synthesis(char file[], int flow_size)
{   
    ifstream is(file, ios::in | ios::binary);
    char buf[2000] = {0};

    for (int i = 1; i <= flow_size; i++)
    {
        if(!is.read(buf, 4))
        {
            cout << "Data Loading Error." << endl;
            break;
        }
        int id = MurmurHash3_x86_32((void*)(buf), 4, 2022);
        flow.push_back(id);
    }

    cout << "Loading complete. flow_size: " << flow.size() << endl;
}

int calc_ts_num_bucket(int size, int level, int threshold, double R_w, double R_l) 
{
    int now_size = size, now_bound = threshold;
    int num_bucket = 0;
    for (int i = 0; i < level; ++i) {
        if (now_size < 1)
        {
            level = i;
            break;
        } 
        num_bucket += now_size;
        now_size = (int)ceil((double)now_size / R_w);
        now_bound = (int)floor((double)now_bound / R_l);
    }
    return num_bucket;
}

int get_ts_num_bucket(double size, int level, int threshold, double R_w, double R_l) 
{
    int L = 1, R = (int)(size * 1024 * 1024 / 8);
    while (L < R) {
        int mid = (L + R) >> 1;
        int num_bucket = calc_ts_num_bucket(mid, level, threshold, R_w, R_l);
        double real_mem = (double)num_bucket * 8 / 1024 / 1024;
        if (real_mem <= size)
            L = mid+1;
        else
            R = mid;
    }
    return L;
}

void FrequencyEst(vector<Sketch*>& sk, vector<int>& flow, int k, int eps, bool init_first = false)
{
    static map<int, int> flow_cnt;
    int n = flow.size(), n_sk = sk.size();
    
    flow_cnt.clear();
    for (int i = 0; i < n; ++i)
        flow_cnt[ flow[i] ] = 0;
    for (int i = 0; i < n; ++i)
        flow_cnt[ flow[i] ]++;

    for (int id = 0; id < n_sk; ++id)
    {
        if (init_first)
            sk[id]->init();

        for (int i = 0; i < n; ++i)
        {
            // printf("insert %d\n", flow[i]);
            sk[id]->insert(flow[i]);
        }
    }

    // summary
    static vector<PII> res;
    res.clear();
    map<int, int>::iterator it = flow_cnt.begin();
    while (it != flow_cnt.end())
    {
        res.push_back(mp((*it).sc, (*it).ft));
        it++;
    }
    sort(res.begin(), res.end(), greater<PII>());
    printf("\n\nFreqEst Test (Based on Top %d)\n", k);
    printf("-------------------------------------\n");

    // fstream fout(file_name, ios::out | ios::app);
    int sz = min(k, (int)res.size());
    for (int id = 0; id < n_sk; ++id)
    {
        printf("%d) %s\n", id, sk[id]->name);

        double lvl_dis[40];
        double avg_hash = 0;
        double are = 0., aae = 0., pr = 0.;
        double over_eps = 0;

        bool is_ts = strcmp(sk[id]->name, "TrustySketch") == 0;

        if (is_ts)
        {
            // printf("  no.:      flow_id\treal\thigh\tlow\tlevel\n");
            for (int j = 0; j <= ((TSketch*)sk[id])->level; ++j)
                lvl_dis[j] = 0;
            for (int i = 0; i < sz; ++i)
            {
                int high = sk[id]->query_freq(res[i].sc);
                int low = sk[id]->query_freq_low(res[i].sc);
                int lvl = ((TSketch*)sk[id])->query_freq_level(res[i].sc);
                lvl_dis[lvl+1] += 1;

                int err = abs(high - res[i].ft);
                are += (double)err / res[i].ft;
                aae += (double)err;
                pr += (high == res[i].ft);
                over_eps += (err > eps);
            }
        }
        else
        {
            for (int i = 0; i < sz; ++i)
            {
                int high = sk[id]->query_freq(res[i].sc);
                // int low = sk[id]->query_freq_low(res[i].sc);
                int low = 0;

                int err = abs(high - res[i].ft);
                are += (double)err / res[i].ft;
                aae += (double)err;
                pr += (high == res[i].ft);
                over_eps += (err > eps);
            }
        }

        sk[id]->status();
        if (is_ts)
        {
            int level = ((TSketch*)sk[id])->level;
            printf("Lvl Distr.: [  ");
            for (int j = 1; j <= level; ++j)
                printf("%.3lf  ", lvl_dis[j]/sz);
            printf("]  conflicting: %.3lf  not recorded: %.3lf\n", lvl_dis[0]/sz, lvl_dis[level+1]/sz);
        }
        // fout  << setiosflags(ios::fixed) << setprecision(5) << over_eps/sz << " " << (int)over_eps << " "; //********
        // fout  << setiosflags(ios::fixed) << setprecision(5) << are/sz << " " << aae/sz << " "; //********
        printf("Pr: %.3lf  ARE: %1.3lf(%.3lf)  AAE: %1.3lf(%.3lf)  OverEps: %1.3lf(%d)\n", pr/sz, are/sz, log10(are/sz), aae/sz, log10(aae/sz), over_eps/sz, (int)over_eps);
    }
    // fout << endl;
}

void Throughput(vector<Sketch*>& sk, vector<int>& flow)
{
    static map<int, int> flow_cnt;
    int n = flow.size(), n_sk = sk.size();
    // fstream fout(file_name, ios::out | ios::app);

    flow_cnt.clear();
    for (int i = 0; i < n; ++i)
        flow_cnt[flow[i]] = 0;
    for (int i = 0; i < n; ++i)
        flow_cnt[flow[i]]++;

    printf("\nThroughput Test\n");
    printf("-------------------------------------\n");

    double mi = (double)n / 1e6;
    for (int id = 0; id < n_sk; ++id)
    {
        // fout << sk[id]->name << " ";
        sk[id]->init();
        Timer timer;
        for (int i = 0; i < n; ++i)
        {
            sk[id]->insert(flow[i]);
        }
        double duration = timer.Finish();
        printf("%s, total insert time = %.3lf, throughput = %.3lf\n", sk[id]->name, duration, mi/duration);
        // fout << setiosflags(ios::fixed) << setprecision(3) << mi/duration << " ";
    }

    printf("flow_cnt = %d\n", flow_cnt.size());
    

    for (int id = 0; id < n_sk; ++id)
    {
        Timer timer;
        for (int i = 0; i < n; ++i)
        {
            int t = sk[id]->query_freq(flow[i]);
        }
        double duration = timer.Finish();
        printf("%s, total query time = %.3lf, throughput = %.3lf\n", sk[id]->name, duration, mi / duration);
        // fout << setiosflags(ios::fixed) << setprecision(3) << mi/duration << " ";
    }
    // fout << endl;
}

int main(int argc, char *argv[])
{
    srand(time(0));

    int flow_size = 1e7;
    // load data
    LoadData_IP("data/ip.dat", flow_size); flow_size = flow.size();
    // LoadData_Web("data/web.dat", flow_size); flow_size = flow.size();
    // LoadData_Click("data/click.dat", flow_size); flow_size = flow.size();
    // flow_size = 1e8; LoadData_Synthesis("data/syn003.dat", flow_size); flow_size = flow.size();
    // flow_size = 1e8; LoadData_Synthesis("data/syn030.dat", flow_size); flow_size = flow.size();

    file_name = string("log2/log_freq_dyeps_web2.txt");

    // binary approach
    fstream fout(file_name, ios::out | ios::app);

    int eps = 50;
    double R_l = 2.5;
    double R_w = 2.;

    // test
    for (int i = 5; i <= 20; i++)
    {
        // memory - MB
        double mem = 0.2 * i;

        // eps = (int) (2. * flow_size * (R_w*R_l) * (R_w*R_l) / (mem*1024*1024/8) / (R_w-1) * (R_l-1));  // dynamic eps for average error
        eps = 25;
        R_l = 2.5;
        R_w = 2.;

        vector<Sketch*> sk;
        int num_bucket = get_ts_num_bucket(mem, 30, ceil((double)eps/R_l*(R_l-1)), R_w, R_l);

        printf("#### Now Eps = %d, memory = %.2lfMB\n", eps, mem);
        TSketch *ts = new TSketch(num_bucket, 30, ceil((double)eps/R_l*(R_l-1)), R_w, R_l);
        int total_bucket = ts->num_bucket;
        CMSketch *cms = new CMSketch(total_bucket*2, 3); cms->set_name("CMSketch(fast)");
        CUSketch *cus = new CUSketch(total_bucket*2, 3); cus->set_name("CUSketch(fast)");
        ASketch *as = new ASketch(total_bucket*2, 4, 256);
        HeavyGuardian *hg = new HeavyGuardian((int)(0.128*total_bucket), 8, 64, 1.08);
        SpaceSaving *ss = new SpaceSaving((int)(0.304*total_bucket));
        CMSketch *cms2 = new CMSketch(total_bucket*2, 16); cms2->set_name("CMSketch(accurate)");
        CUSketch *cus2 = new CUSketch(total_bucket*2, 16); cus2->set_name("CMSketch(accurate)");
        sk.push_back(ts);
        sk.push_back(cms);
        sk.push_back(cus);
        sk.push_back(as);
        sk.push_back(ss);
        sk.push_back(hg);
        sk.push_back(cms2);
        sk.push_back(cus2);
        FrequencyEst(sk, flow, flow_size, eps, true);
        // Throughput(sk, flow);
        delete ts;
        delete cms;
        delete cus;
        delete as;
        delete hg;
        delete ss;
        delete cms2;
        delete cus2;
    }

    return 0;
}
