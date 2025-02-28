#ifndef MATERIAL_H
#define MATERIAL_H

#include <GL/glew.h>
#include <vector>
#include <string>

// Define the MaterialProperty struct
struct MaterialProperty 
{
    GLfloat mat_ambient[4];  // Array for ambient color (RGBA)
    GLfloat mat_diffuse[4];  // Array for diffuse color (RGBA)
    GLfloat mat_specular[4]; // Array for specular color (RGBA)
    GLfloat mat_shininess;   // Shininess value

    // Constructor to set material properties
    MaterialProperty(GLfloat amb_r = 1.0f, GLfloat amb_g = 0.5f, GLfloat amb_b = 0.31f, GLfloat amb_a = 1.0f,
        GLfloat diff_r = 1.0f, GLfloat diff_g = 0.5f, GLfloat diff_b = 0.31f, GLfloat diff_a = 1.0f,
        GLfloat spec_r = 1.0f, GLfloat spec_g = 0.5f, GLfloat spec_b = 0.31f, GLfloat spec_a = 1.0f,
        GLfloat shine = 64.0f) {
        mat_ambient[0] = amb_r;
        mat_ambient[1] = amb_g;
        mat_ambient[2] = amb_b;
        mat_ambient[3] = amb_a;

        mat_diffuse[0] = diff_r;
        mat_diffuse[1] = diff_g;
        mat_diffuse[2] = diff_b;
        mat_diffuse[3] = diff_a;

        mat_specular[0] = spec_r;
        mat_specular[1] = spec_g;
        mat_specular[2] = spec_b;
        mat_specular[3] = spec_a;

        mat_shininess = shine;
    }
};

// Enum for predefined material types
enum class MaterialType 
{
    DEFAULT,
    EMERALD,
    JADE,
    OBSIDIAN,
    PEARL,
    RUBY,
    TURQUOISE,
    BRASS,
    BRONZE,
    CHROME,
    COPPER,
    GOLD,
    SILVER,
    BLACK_PLASTIC,
    CYAN_PLASTIC,
    GREEN_PLASTIC,
    RED_PLASTIC,
    WHITE_PLASTIC,
    YELLOW_PLASTIC,
    BLACK_RUBBER,
    CYAN_RUBBER,
    GREEN_RUBBER,
    RED_RUBBER,
    WHITE_RUBBER,
    YELLOW_RUBBER
};

// Declare the array of material type names for ImGui dropdown
extern std::vector<std::string> material_types;

// Declare the function to get MaterialProperty for a given MaterialType
MaterialProperty getMaterialProperties(MaterialType type);

#endif // MATERIAL_H
