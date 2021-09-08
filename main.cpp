#include <iostream>
#include <string>
#include <cpr/cpr.h>


int countChars(const std::string &s, char c) {
    int count = 0;
    for (char v:s) {
        count += (int)(v == c);
    }

    return count;
}

void stringToPair(std::string &s, std::vector<cpr::Pair> &p) {
    std::string first;
    std::string second;

    int delimeterPos = s.find(' ');
    first = s.substr(0, delimeterPos);
    second = s.substr(delimeterPos + 1, s.length() - (delimeterPos-1));

    //std::cout << first << ":" << second << std::endl;
    p.emplace_back((std::string)first, (std::string)second);

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

    bool get(std::vector<cpr::Pair> &p) {
        if (this->url.length() == 0) return false;

        std::string params = "?";
        for (auto pair = p.begin(); pair != p.end(); ++pair) {
            auto arg = pair->key;
            auto val = pair->value;

            params += arg;
            params += "=";
            params += val;
            params += "&";
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

    bool post(std::vector<cpr::Pair> &p) {
        if (this->url.length() == 0) return false;

        std::string tmp_url = this->url+"/post";
        r = cpr::Post(cpr::Url(tmp_url), cpr::Payload(p.begin(), p.end()));

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

    std::vector<cpr::Pair> requestsVec;
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

        stringToPair(request, requestsVec);
        std::cin.clear();
    }

    return 0;
}
