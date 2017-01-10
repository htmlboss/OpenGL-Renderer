#pragma once
/*
    Remember: to specify vertices in a counter-clockwise winding order you need to visualize the triangle
    as if you're in front of the triangle and from that point of view, is where you set their order.
    
    To define the order of a triangle on the right side of the cube for example, you'd imagine yourself looking
    straight at the right side of the cube, and then visualize the triangle and make sure their order is specified
    in a counter-clockwise order. This takes some practice, but try visualizing this yourself and see that this
    is correct.
*/

const float vertices[] = {
    // Back face
    -0.5f, -0.5f, -0.5f,  // Bottom-left
     0.5f,  0.5f, -0.5f,  // top-right
     0.5f, -0.5f, -0.5f,  // bottom-right         
     0.5f,  0.5f, -0.5f,  // top-right
    -0.5f, -0.5f, -0.5f,  // bottom-left
    -0.5f,  0.5f, -0.5f,  // top-left
    // Front face
    -0.5f, -0.5f,  0.5f,  // bottom-left
     0.5f, -0.5f,  0.5f,  // bottom-right
     0.5f,  0.5f,  0.5f,  // top-right
     0.5f,  0.5f,  0.5f,  // top-right
    -0.5f,  0.5f,  0.5f,  // top-left
    -0.5f, -0.5f,  0.5f,  // bottom-left
    // Left face
    -0.5f,  0.5f,  0.5f,  // top-right
    -0.5f,  0.5f, -0.5f,  // top-left
    -0.5f, -0.5f, -0.5f,  // bottom-left
    -0.5f, -0.5f, -0.5f,  // bottom-left
    -0.5f, -0.5f,  0.5f,  // bottom-right
    -0.5f,  0.5f,  0.5f,  // top-right
    // Right face
     0.5f,  0.5f,  0.5f, // top-left
     0.5f, -0.5f, -0.5f, // bottom-right
     0.5f,  0.5f, -0.5f, // top-right         
     0.5f, -0.5f, -0.5f, // bottom-right
     0.5f,  0.5f,  0.5f, // top-left
     0.5f, -0.5f,  0.5f, // bottom-left     
    // Bottom face
    -0.5f, -0.5f, -0.5f, // top-right
     0.5f, -0.5f, -0.5f, // top-left
     0.5f, -0.5f,  0.5f, // bottom-left
     0.5f, -0.5f,  0.5f, // bottom-left
    -0.5f, -0.5f,  0.5f, // bottom-right
    -0.5f, -0.5f, -0.5f, // top-right
    // Top face
    -0.5f,  0.5f, -0.5f, // top-left
     0.5f,  0.5f,  0.5f, // bottom-right
     0.5f,  0.5f, -0.5f, // top-right     
     0.5f,  0.5f,  0.5f, // bottom-right
    -0.5f,  0.5f, -0.5f, // top-left
    -0.5f,  0.5f,  0.5f  // bottom-left        
};