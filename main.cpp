#include <iostream>
#include <string>
#include <cpr/cpr.h>


struct parameters {
    std::string arg = "";
    std::string value = "";
};

int countChars(const std::string &s, char c) {
    int count = 0;
    for (char v:s) {
        count += (int)(v == c);
    }

    return count;
}

void stringToPair(std::string &s, parameters &p) {
    std::string first;
    std::string second;

    int delimeterPos = s.find(' ');
    first = s.substr(0, delimeterPos);
    second = s.substr(delimeterPos + 1, s.length() - (delimeterPos-1));

    //std::cout << first << ":" << second << std::endl;
    p.arg = first;
    p.value = second;
}

class Requests {
private:
    std::string url;
    cpr::Response r;
public:
    Requests(const std::string &val) {
        if (val.length() == 0) return;

        this->url = val;
    }

    bool get(std::vector<parameters> &p) {
        if (this->url.length() == 0) return false;

        std::string params = "?";
        for (int i = 0; i < p.size(); i ++) {
            auto arg = p[i].arg;
            auto val = p[i].value;

            params += arg;
            params += "=";
            params += val;

            if (i < p.size() - 1) params += "&";
        }

        std::string tmp_url = this->url+"/get"+params;

        r = cpr::Get(cpr::Url(tmp_url));

        if (r.status_code != 200) {
            this->getErrors();
            return false;
        }

        std::cout << r.text << std::endl;
        return true;
    }

    bool post(std::vector<parameters> &p) {
        if (this->url.length() == 0) return false;
        cpr::Payload pl = {};
        for (auto & i : p) {
            std::string arg = i.arg;
            std::string val = i.value;

            cpr::Pair pair(arg.c_str(), val.c_str());

            cpr::CurlHolder holder;
            pl.AddPair(pair, holder);
        }

        std::string tmp_url = this->url+"/post";
        r = cpr::Post(cpr::Url(tmp_url), cpr::Payload(pl));

        if (r.status_code != 200) {
            this->getErrors();
            return false;
        }

        std::cout << r.text << std::endl;
        return true;
    }

    void getErrors() {
        if (r.status_code == 200) return;

        if (r.status_code == 0) {
            std::cerr << r.error.message << std::endl;
        } else if (r.status_code == 400) {
            std::cerr << "Error [" << r.status_code << "] making request" << std::endl;
        }
    }


};

int main() {
    std::cout << "CPR 30.3 requests with arguments" << std::endl;

    std::vector<parameters> requestsVec;
    Requests *r = new Requests("http://httpbin.org/");
    while (true) {
        std::string request;
        std::getline(std::cin, request);

        if (request == "get" || request == "GET") {
            r->get(requestsVec);
            break;
        } else if (request == "post" || request == "POST") {
            r->post(requestsVec);
            break;
        }

        if (countChars(request, ' ') != 1) {
            std::cout << "Error pair." << std::endl;
            continue;
        }

        parameters p;
        stringToPair(request, p);
        requestsVec.push_back(p);

        std::cin.clear();
    }
    return 0;
}
