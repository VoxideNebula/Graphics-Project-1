#ifndef _CSCI441_SHAPE_H_
#define _CSCI441_SHAPE_H_

#include <cstdlib>
#include <vector>

template <typename T, typename N, typename C>
void add_vertex(T& coords, const N& x, const N& y, const N& z,
        const C& r, const C& g, const C& b,
        bool with_noise=false) {
    // adding color noise makes it easier to see before shading is implemented
    float noise = 1-with_noise*(rand()%150)/100.;

    // x y z
    coords.push_back(x);
    coords.push_back(y);
    coords.push_back(z);

    // r g b
    coords.push_back(r*noise);
    coords.push_back(g*noise);
    coords.push_back(b*noise);

    // norm x y z
    coords.push_back(x);
    coords.push_back(y);
    coords.push_back(z);
}

class Sphere {
    double x(float r, float phi, float theta){
        return r*cos(theta)*sin(phi);
    }

    double y(float r, float phi, float theta){
        return r*sin(theta)*sin(phi);
    }

    double z(float r, float phi, float theta){
        return r*cos(phi);
    }

public:
    std::vector<float> coords;
    Sphere(unsigned int n, float radius, float r, float g, float b) {
        int n_steps = (n%2==0) ? n : n+1;
        double step_size = 2*M_PI / n_steps;

        for (int i = 0; i < n_steps; ++i) {
            for (int j = 0; j < n_steps/2.0; ++j) {
                double phi_i = i*step_size;
                double phi_ip1 = ((i+1)%n_steps)*step_size;
                double theta_j = j*step_size;
                double theta_jp1 = ((j+1)%n_steps)*step_size;

                // vertex i,j
                double vij_x = x(radius, phi_i, theta_j);
                double vij_y = y(radius, phi_i, theta_j);
                double vij_z = z(radius, phi_i, theta_j);

                // vertex i+1,j
                double vip1j_x = x(radius, phi_ip1, theta_j);
                double vip1j_y = y(radius, phi_ip1, theta_j);
                double vip1j_z = z(radius, phi_ip1, theta_j);

                // vertex i,j+1
                double vijp1_x = x(radius, phi_i, theta_jp1);
                double vijp1_y = y(radius, phi_i, theta_jp1);
                double vijp1_z = z(radius, phi_i, theta_jp1);

                // vertex i+1,j+1
                double vip1jp1_x = x(radius, phi_ip1, theta_jp1);
                double vip1jp1_y = y(radius, phi_ip1, theta_jp1);
                double vip1jp1_z = z(radius, phi_ip1, theta_jp1);

                // add triangle
                add_vertex(coords, vij_x, vij_y, vij_z, r, g, b);
                add_vertex(coords, vip1j_x, vip1j_y, vip1j_z, r, g, b);
                add_vertex(coords, vijp1_x, vijp1_y, vijp1_z, r, g, b);

                // add triange
                add_vertex(coords, vijp1_x, vijp1_y, vijp1_z, r, g, b);
                add_vertex(coords, vip1jp1_x, vip1jp1_y, vip1jp1_z, r, g, b);
                add_vertex(coords, vip1j_x, vip1j_y, vip1j_z, r, g, b);
            }
        }
    }
};

#endif
