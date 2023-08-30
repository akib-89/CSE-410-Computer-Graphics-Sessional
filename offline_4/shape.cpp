/**
 * This file contains the basic definition and implementation of the shape class
 */

#ifndef SHAPE_H
#define SHAPE_H

#include <cmath>
#include <vector>

#include "color.cpp"
#include "light.cpp"
#include "line.cpp"
#include "spot_light.cpp"
#include "vector3d.cpp"

class Shape {
 protected:
  Vector3D position;              // center of the shape
  Color color;                    // color of the shape
  double ambient_coefficient;     // ka
  double diffuse_coefficient;     // kd
  double specular_coefficient;    // ks
  double reflection_coefficient;  // for metallic reflection
  int specular_exponent;          // for specular reflection(exponent)

 public:
  Shape()
      : ambient_coefficient(0.0),
        diffuse_coefficient(0.0),
        specular_coefficient(0.0),
        reflection_coefficient(0.0),
        specular_exponent(1) {}

  Shape(Vector3D position,
        Color color,
        double ambient_coefficient,
        double diffuse_coefficient,
        double specular_coefficient,
        double reflection_coefficient,
        int specular_exponent)
      : position(position),
        color(color),
        ambient_coefficient(ambient_coefficient),
        diffuse_coefficient(diffuse_coefficient),
        specular_coefficient(specular_coefficient),
        reflection_coefficient(reflection_coefficient),
        specular_exponent(specular_exponent) {}

  Vector3D getPosition() { return position; }
  Color getColor() { return color; }
  double getAmbientCoefficient() { return ambient_coefficient; }
  double getDiffuseCoefficient() { return diffuse_coefficient; }
  double getSpecularCoefficient() { return specular_coefficient; }
  double getReflectionCoefficient() { return reflection_coefficient; }

  void setPosition(Vector3D position) { this->position = position; }
  void setColor(Color color) { this->color = color; }
  void setAmbientCoefficient(double ambient_coefficient) {
    this->ambient_coefficient = ambient_coefficient;
  }
  void setDiffuseCoefficient(double diffuse_coefficient) {
    this->diffuse_coefficient = diffuse_coefficient;
  }
  void setSpecularCoefficient(double specular_coefficient) {
    this->specular_coefficient = specular_coefficient;
  }
  void setReflectionCoefficient(double reflection_coefficient) {
    this->reflection_coefficient = reflection_coefficient;
  }
  double intersect(Line& line,
                   vector<Light*> lights,
                   vector<SpotLight*> spot_lights,
                   vector<Shape*> shapes,
                   Color& color_to_return,
                   int current_level,
                   int recursion_level) {
    double t = getIntersection(line);
    if (t < 0) {
      return -1;  // no intersection
    }
    if (recursion_level == 0) {
      return t;
    }
    // get the intersection point
    Vector3D intersection_point = line.getPoint(t);
    // get the color at the intersection point
    Color color_at_intersection_point = getColorAt(intersection_point);
    // update the color value with ambient light
    Color color_value = color_at_intersection_point * ambient_coefficient;

    // for each light source
    for (int i = 0; i < lights.size(); i++) {
      // get the light position and direction
      Vector3D light_position = lights[i]->getPosition();
      Vector3D light_direction = light_position - intersection_point;

      // generate a new line from the light source to the intersection point
      Line light_line(light_position, light_direction);

      // get the normal at the intersection point
      Line normal_line = getNormal(intersection_point, light_line);
      // find the scaling factor for the intersection point
      // exp(-distance*distance*S.falloff);
      double scaling_factor =
          exp(-1 * light_direction.length() * light_direction.length() *
              lights[i]->getFalloff());

      // check if the light source is visible from the intersection point
      bool is_visible = true;
      for (int j = 0; j < shapes.size(); j++) {
        double other_t = shapes[j]->getIntersection(light_line);
        if (other_t > 0 && other_t < t - 0.0001) {
          is_visible = false;
          break;
        }
      }

      // if the light source is visible from the intersection point
      if (is_visible) {
        // lambertian shading
        double lambart_component = (normal_line.getDirection() * (-1))
                                       .dot_product(light_line.getDirection());
        if (lambart_component < 0) {
          lambart_component = 0;
        }

        // phong shading
        // ar first, we need to get the reflection vector
        double dot_product =
            normal_line.getDirection().dot_product(light_line.getDirection());
        Vector3D reflection_vector =
            light_line.getDirection() -
            normal_line.getDirection() * 2 * dot_product;

        double phong_component =
            (line.getDirection() * (-1)).dot_product(reflection_vector);
        if (phong_component < 0) {
          phong_component = 0;
        }

        // update the color value with diffuse and specular light
        color_value =
            color_value + color_at_intersection_point *
                              (lights[i]->getColor() * diffuse_coefficient *
                                   lambart_component +
                               lights[i]->getColor() * specular_coefficient *
                                   pow(phong_component, specular_exponent)) *
                              scaling_factor;

        // update the color to return
        color_to_return = color_to_return + color_value;
      }
    }

    // same for spot light
    for (int i = 0; i < spot_lights.size(); i++) {
      // get the light position and direction
      Vector3D light_position = spot_lights[i]->getPosition();
      Vector3D light_direction = light_position - intersection_point;

      // generate a new line from the light source to the intersection point
      Line light_line(light_position, light_direction);

      // get the normal at the intersection point
      Line normal_line = getNormal(intersection_point, light_line);
      // find the scaling factor for the intersection point
      // exp(-distance*distance*S.falloff);
      double scaling_factor =
          exp(-1 * light_direction.length() * light_direction.length() *
              spot_lights[i]->getFalloff());

      // check if the light source is visible from the intersection point
      bool is_visible = true;
      for (int j = 0; j < shapes.size(); j++) {
        double other_t = shapes[j]->getIntersection(light_line);
        if (other_t > 0 && other_t < t - 0.0001) {
          is_visible = false;
          break;
        }
      }

      // another extra check for spot light
      // check if the light source is within the cone of the spot light
      // (the angle between the light direction and the spot light direction is
      // < spot light angle)
      double angle = spot_lights[i]->getDirection().angle(light_direction);
      // angle is in radian
      if (angle * 180 / M_PI > spot_lights[i]->getAngle()) {
        is_visible = false;
      }

      // if the light source is visible from the intersection point
      if (is_visible) {
        // lambertian shading
        double lambart_component = (normal_line.getDirection() * (-1))
                                       .dot_product(light_line.getDirection());
        if (lambart_component < 0) {
          lambart_component = 0;
        }

        // phong shading
        // ar first, we need to get the reflection vector
        double dot_product =
            normal_line.getDirection().dot_product(light_line.getDirection());
        Vector3D reflection_vector =
            light_line.getDirection() -
            normal_line.getDirection() * 2 * dot_product;

        double phong_component =
            (line.getDirection() * (-1)).dot_product(reflection_vector);
        if (phong_component < 0) {
          phong_component = 0;
        }

        // update the color value with diffuse and specular light
        color_value = color_value +
                      color_at_intersection_point *
                          (spot_lights[i]->getColor() * diffuse_coefficient *
                               lambart_component +
                           spot_lights[i]->getColor() * specular_coefficient *
                               pow(phong_component, specular_exponent)) *
                          scaling_factor;

        // update the color to return
        color_to_return = color_to_return + color_value;
      }
    }

    // reflection

    return getIntersection(line);
  }

  virtual Line getNormal(Vector3D& intersection_point, Line line) = 0;
  // todo: change the method name and parameters of this method
  virtual double getIntersection(Line& line) = 0;
  virtual Color getColorAt(Vector3D& intersection_point) = 0;
  virtual void draw() = 0;
};

#endif  // SHAPE_H
