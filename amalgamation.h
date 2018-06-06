#ifndef AMALGAMATION_H
#define AMALGAMATION_H

#include <iostream>
#include <map>
#include <string>
#include <fstream>
#include <vector>
#include <regex>
#include <set>
#include <math.h>

using namespace std;


typedef pair<string,string> coord_position;
typedef map<string, double> column_matrix_cluster;
typedef map< string, column_matrix_cluster > matrix_cluster;
typedef map<string, vector<double>> map_coords;

typedef pair<double, string> coord_distance;


std::vector<double> split(const string& input, const string& regex, string &label) {
    // passing -1 as the submatch index parameter performs splitting
    std::regex re(regex);
    std::sregex_token_iterator
            first{input.begin(), input.end(), re, -1},
    last;
    vector<string> ret = {first, last};
    vector<double> ret_double;
    label = ret[0];
    ret.erase(ret.begin());
    for(auto n:ret){
        ret_double.push_back(stod(n));
    }
    return ret_double;
}

class LinkageOperation {
public:
  LinkageOperation(){}
  virtual double operator() (double& left, double& right) = 0;
};

class SimpleLinkage: public LinkageOperation
{
public:
    double operator()(double& left, double& right)
    {
        return left<right ? left : right;
    }
};


struct CmpMinimum
{
    bool operator()(const pair<string,double>& left, const pair<string,double>& right) const
    {
        if(left.second == 0 && right.second != 0){
            return 0;
        }
        else if(left.second != 0 && right.second == 0){
            return 1;
        }
        return left.second < right.second;
    }
};

class LinkageCluster
{
public:
    matrix_cluster matrix;
    map_coords coords;
    double minimum_value;

    coord_position minimum_position;

    void update_minimum(){
        int i=1;
        minimum_value = std::numeric_limits<double>::infinity();
        for (auto it = next(matrix.begin(),1); it != matrix.end(); ++it)
        {
            pair<string, double> min = *min_element((*it).second.begin(), next((*it).second.begin(),i) , CmpMinimum());
            if(min.second < minimum_value){
                minimum_value = min.second;
                this->minimum_position = make_pair((*it).first, min.first);
            }
            i++;
        }
    }

    set<coord_distance> get_minimums_disociative(){
        set<coord_distance> ret;
        for (auto &row : matrix)
        {
            pair<string, double> min = *min_element(row.second.begin(), row.second.end() , CmpMinimum());
            ret.insert(make_pair(min.second, row.first));
        }
        return ret;
    }

    void print_values_map(string key){
        for (auto &x : matrix[key])
        {
            cout << x.second << '\t';
        }
    }
    void print_matrix(){
        cout << '\t';
        for (auto &x : matrix)
        {
            cout << x.first << '\t';
        }
        cout << endl;
        for (auto &x : matrix)
        {
            cout << x.first << '\t';
            print_values_map(x.first);
            cout << endl;
        }
        cout << endl;
    }

    double euclidean_distance(vector<double> a_1, vector<double> a_2){
        double ret = 0;
        for(int i=0; i<a_1.size(); i++){
            ret += pow((a_1[i]-a_2[i]),2);
        }
        return sqrt(ret);
    }
    void add_to_matrix(string &label, vector<double> &weights){
        for (auto &row : matrix)
        {
            double distance = euclidean_distance(weights, coords[row.first]);
            matrix[row.first][label] = distance;
            matrix[label][row.first] = matrix[row.first][label];
        }
        matrix[label][label] = 0;
    }

    LinkageCluster(string file_name) {
        std::ifstream file(file_name.c_str());
        std::string str;
        while (std::getline(file, str))
        {
            string label;
            vector<double> weights = split(str, string("\t"), label);
            coords[label] = weights;
            add_to_matrix(label, weights);
        }
        update_minimum();

    }

    void make_clustering(int k, LinkageOperation* operation){
        for(int i=0;i<=k;i++){
            string first_target = minimum_position.first;
            string second_target = minimum_position.second;
            string final_target = first_target + "-" + second_target;

            column_matrix_cluster map_first_target = matrix[first_target];
            column_matrix_cluster map_second_target = matrix[second_target];

            matrix.erase(first_target);
            matrix.erase(second_target);

            for(auto &row: matrix){
                if(row.first == final_target){
                    continue;
                }
                row.second.erase(first_target);
                row.second.erase(second_target);
                double target_value = (*operation)(map_first_target[row.first], map_second_target[row.first]);
                matrix[row.first][final_target] = target_value;
                matrix[final_target][row.first] = target_value;
            }
            matrix[final_target][final_target] = 0;
            update_minimum();
        }
    }

    void disociative_clustering(int k){
        set<coord_distance> minimums = get_minimums_disociative();
        vector<string> clusters;
        while(clusters.size()+1 < k){
//            if(minimums.size() <= 2){
//                auto first = *minimums.begin();
//                auto last = *--minimums.end();
//                clusters.push_back(first.second + "-" + last.second);
//                break;
//            }
            coord_distance major = *--minimums.end();
            minimums.erase(--minimums.end());
            string cluster_name = major.second;
            for(auto &it: minimums){
                double difference = it.first - matrix[major.second][it.second];
                if(difference >= 0){
                    cluster_name += "-" + it.second;
                    minimums.erase(it);
                }
            }
            clusters.push_back(cluster_name);
        }
        int i=1;
        for(auto &it: clusters){
            cout << "Cluster " << i << ":" << endl;
            cout << it << endl;
            i++;
        }
        string final_cluster = "";
        for(auto &mins: minimums){
            final_cluster += mins.second;
        }
        cout << "Cluster " << i << ":" << endl;
        cout << final_cluster << endl;
    }

};



#endif // AMALGAMATION_H
