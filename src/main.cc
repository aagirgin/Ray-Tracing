#include "rtweekend.h"
#include "camera.h"
#include "color.h"
#include "hittable_list.h"
#include "sphere.h"
#include "material.h"
#include <fstream>
#include <iostream>
#include "box.h"

color ray_color(const ray& r, const color& background, const hittable& world, int depth) {
    hit_record rec;
    if (depth <= 0)
    {
        return color(0,0,0);
    }

    if (!world.hit(r, 0.001, infinity, rec))
    {
        return background;
    }


    ray scattered;
    color attenuation;

    color emitted = rec.mat_ptr->emitted(rec.u, rec.v, rec.p);

    if (!rec.mat_ptr->scatter(r, rec, attenuation, scattered))
        return emitted;

    return emitted + attenuation * ray_color(scattered, background, world, depth-1);
}

int main() {

    std::ofstream inFile("image.ppm");

    // Image

    const auto aspect_ratio = 16.0 / 9.0;
    const int image_width = 1920;
    const int image_height = static_cast<int>(image_width / aspect_ratio);
    const int samples_per_pixel = 100;
    const int max_depth = 5;
    // World

    hittable_list world;

    auto dielectric_material   = make_shared<dielectric>(1.5);
    auto material_ground = make_shared<lambertian>(color(0.8, 0.8, 0.0));
    auto material_center = make_shared<lambertian>(color(0.7, 0.3, 0.3));
    auto material_left   = make_shared<metal>(color(0.8, 0.8, 0.8));
    auto material_right  = make_shared<metal>(color(0.8, 0.6, 0.2));
    auto light_material  = make_shared<diffuse_light>(color(0, 0, 0.2));

    world.add(make_shared<sphere>(point3( 0.0, -100.5, -1.0), 100.0, material_ground));

    world.add(make_shared<box>(point3(0, 0, -1), point3(10, 10, 10), material_center));

    world.add(make_shared<sphere>(point3( -50,  50 , -5.0),   10, light_material)); 

    // n
    for (int i = 0.0; i < 10;i++)
    {
        world.add(make_shared<sphere>(point3( i-5,    i , -3.0),   0.8, material_right));
    }
    // reverse n

    for (int i = 0.0; i < 10;i++)
    {
        world.add(make_shared<sphere>(point3( 5 - i,  i , -3.0),   0.8, material_left)); 
    }

    // n2
    for (int i = 0.0; i < 10;i++)
    {
        world.add(make_shared<sphere>(point3( i-5,    i , -13.0),   0.8, material_right));
    }
    // reverse n

    for (int i = 0.0; i < 10;i++)
    {
        world.add(make_shared<sphere>(point3( 5 - i,  i , -13.0),   0.8, material_left)); 
    }


    // n3 left
    for (int i = 0.0; i < 10;i++)
    {
        world.add(make_shared<sphere>(point3( -5,   i , i - 13.0),   0.8, material_right));
    }
    // reverse n

    for (int i = 0.0; i < 10;i++)
    {
        world.add(make_shared<sphere>(point3( -5 ,  i , - 3.0 - i),   0.8, material_left)); 
    }

        // n4 left
    for (int i = 0.0; i < 10;i++)
    {
        world.add(make_shared<sphere>(point3( 5,   i , i - 13.0),   0.8, material_right));
    }
    // reverse n

    for (int i = 0.0; i < 10;i++)
    {
        world.add(make_shared<sphere>(point3( 5 ,  i , -3.0 - i),   0.8, material_left)); 
    }



    /*


    world.add(make_shared<sphere>(point3(0,0,-1), 0.3));
    world.add(make_shared<sphere>(point3(1,0,-1), 0.3));
    world.add(make_shared<sphere>(point3(0,1,-1), 0.3));
    world.add(make_shared<sphere>(point3(-1,0,-1), 0.3));
    world.add(make_shared<sphere>(point3(0,-1,-1), 0.3));
    world.add(make_shared<sphere>(point3(0,-100.5,-1), 100)); // ground

    
    */

    // Camera

    point3 lookfrom(-12,18,10);
    point3 lookat(-2,0,-1);
    vec3 vup(0,1,0);
    auto dist_to_focus =  10.0;
    auto aperture = 0.1;

    camera cam(lookfrom, lookat, vup, 100, aspect_ratio, aperture, dist_to_focus);

    //camera cam(point3(-6,2,5), point3(-2,0,-1), vec3(0,1,0), 100, aspect_ratio);

    color background(0.7, 0.7, 0.7);

    // Render

    inFile << "P3\n" << image_width << ' ' << image_height << "\n255\n";

    for (int j = image_height-1; j >= 0; --j) {
        std::cerr << "\rScanlines remaining: " << j << ' ' << std::flush;
        for (int i = 0; i < image_width; ++i) {
                   color pixel_color(0, 0, 0);
            for (int s = 0; s < samples_per_pixel; ++s) {
                auto u = (i + random_double()) / (image_width-1);
                auto v = (j + random_double()) / (image_height-1);
                ray r = cam.get_ray(u, v);
                pixel_color += ray_color(r, background, world, max_depth);
            }
            write_color(inFile, pixel_color, samples_per_pixel);
        }
    }

    std::cerr << "\nDone.\n";
    inFile.close();

}