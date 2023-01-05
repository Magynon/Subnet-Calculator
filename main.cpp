#include <iostream>
#include <vector>
#include <complex>
#include <algorithm>
#include <cstring>

typedef struct {
    int a, b, c, d;
    int mask;
} ip_address;

bool unlimitedSpace = false;

bool validateIP(ip_address ipAddress) {
    if (ipAddress.a < 0 || ipAddress.a > 255) return false;
    if (ipAddress.b < 0 || ipAddress.b > 255) return false;
    if (ipAddress.c < 0 || ipAddress.c > 255) return false;
    if (ipAddress.d < 0 || ipAddress.d > 255) return false;
    if (ipAddress.mask < 0 || ipAddress.mask > 32) return false;
    return true;
}

ip_address nextAddress(ip_address ip, int hosts) {
    ip.d += hosts;
    if (ip.d > 255) {
        ip.c += ip.d / 256;
        ip.d %= 256;
        if (ip.c > 255) {
            ip.b += ip.c / 256;
            ip.c %= 256;
            if (ip.b > 255) {
                ip.a += ip.b / 256;
                ip.b %= 256;
            }
        }
    }
    return ip;
}

ip_address previousAddress(ip_address ip) {
    ip.d--;
    if (ip.d < 0) {
        ip.d = 255;
        ip.c--;
        if (ip.c < 0) {
            ip.c = 255;
            ip.b--;
            if (ip.b < 0) {
                ip.b = 255;
                ip.a--;
            }
        }
    }
    return ip;
}

int main() {
    ip_address ip_space;

    std::cout << "Input the base ip address (a.b.c.d/e): ";
    std::string base_ip;
    std::cin >> base_ip;

    if (strstr(base_ip.c_str(), "/") == nullptr) {
        std::cout << "WARNING: no mask provided. Assuming unlimited space." << std::endl;
        unlimitedSpace = true;
        sscanf(base_ip.c_str(), "%d.%d.%d.%d", &ip_space.a, &ip_space.b, &ip_space.c, &ip_space.d);
        ip_space.mask = 0; // default mask
    } else {
        sscanf(base_ip.c_str(), "%d.%d.%d.%d/%d", &ip_space.a, &ip_space.b, &ip_space.c, &ip_space.d, &ip_space.mask);
    }

    if (!validateIP(ip_space)) {
        std::cout << "Invalid IP address" << std::endl;
        return 0;
    }

    double total_theoretical_hosts = pow(2, 32 - ip_space.mask);

    std::cout << "Input the number of subnets: ";
    int n;
    double total_hosts = 0;
    std::cin >> n;
    std::vector<std::pair<int, int>> subnets(n);

    for (int i = 0; i < n; i++) {
        std::cout << "Input the number of hosts for subnet " << i + 1 << " (not including network and broadcast addresses): ";
        int hosts;
        std::cin >> hosts;
        total_hosts += hosts + 2;
        int mask = 32 - (int)ceil(log2(hosts + 2));

        std::pair<int, int> subnet;
        subnet.first = mask;
        subnet.second = hosts;
        subnets[i] = subnet;
    }

    if (total_hosts > total_theoretical_hosts) {
        std::cout << std::endl << "WARNING: The number of hosts exceeds the theoretical maximum. Continue? [Y/n]: ";
        std::string ans;

        // empty input buffer
        std::getline(std::cin, ans);

        // read the answer
        std::getline(std::cin, ans);

        if (ans == "n") {
            std::cout << "Aborting..." << std::endl;
            return 0;
        }
    }

    // sort the subnets in descending order
    std::sort(subnets.begin(), subnets.end(), [](std::pair<int, int> a, std::pair<int, int> b) {
        return a.second > b.second;
    });

    ip_address network = ip_space, broadcast, first_ip, last_ip;

    for (int i = 0; i < n; i++) {
        network.mask = subnets[i].first;
        broadcast.mask = subnets[i].first;

        first_ip = nextAddress(network, 1);
        broadcast = nextAddress(network, pow(2, 32 - network.mask) - 1);
        last_ip = previousAddress(broadcast);

        std::cout << std::endl << "Subnet " << i + 1 << " -> " << subnets[i].second
            << " hosts ( +2 = " << subnets[i].second + 2 << " ) < 2^"
            << 32 - network.mask << " = " << pow(2, 32 - network.mask) << " :" << std::endl;
        std::cout << "\t  NETWORK: "
            << network.a << "." << network.b << "." << network.c << "." << network.d << "/" << network.mask << std::endl;
        std::cout << "\tBROADCAST: "
                  << broadcast.a << "." << broadcast.b << "." << broadcast.c << "." << broadcast.d << "/" << broadcast.mask << std::endl;
        std::cout << "\t    first: "
            << first_ip.a << "." << first_ip.b << "." << first_ip.c << "." << first_ip.d << std::endl;
        std::cout << "\t     last: "
            << last_ip.a << "." << last_ip.b << "." << last_ip.c << "." << last_ip.d << std::endl;

        network = nextAddress(broadcast, 1);
    }

    std::cout << std::endl << "DONE..." << std::endl;

    return 0;
}
