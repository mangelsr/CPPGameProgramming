# Assessment 1

## Objective

Create a C++ program that displays a window and renders figures defined in a configuration file.

---

## Configuration File Format

The configuration file will define the window properties, font, and a list of figures to render. Below is an example of the configuration file:

```
Window 800 600
Font font/tech.ttf 18 255 255 255
Circle CGreen 100 100 -0.03 0.02 0 255 0 50
Circle CBlue 200 200 0.02 0.04 255 0 0 255 100
Circle CPurple 300 300 -0.02 -0.01 255 0 255 75
Rectangle RRed 200 200 0.1 0.15 255 0 0 50 25
Rectangle RGray 300 250 -0.02 0.02 100 100 100 50 100
Rectangle RTeal 25 100 -0.02 -0.02 0 255 255 100 100
```

### Details

1. **Window Definition:**
   - `Window <width> <height>`
     - Specifies the dimensions of the window in pixels.

2. **Font Definition:**
   - `Font <path_to_font> <size> <R> <G> <B>`
     - Specifies the font file path, font size, and font color in RGB.

3. **Figures List:**
   - Each line represents a figure to render.
   - The format varies depending on the shape:

#### Circle

- `Circle <label> <x> <y> <speed_x> <speed_y> <R> <G> <B> <radius>`
  - **Label:** Text displayed in the center of the circle.
  - **Position:** Initial position `(x, y)`.
  - **Movement Speed:** Horizontal and vertical speed `(speed_x, speed_y)`.
  - **Color:** RGB values `(R, G, B)`.
  - **Radius:** Size of the circle.

#### Rectangle

- `Rectangle <label> <x> <y> <speed_x> <speed_y> <R> <G> <B> <width> <height>`
  - **Label:** Text displayed in the center of the rectangle.
  - **Position:** Initial position `(x, y)`.
  - **Movement Speed:** Horizontal and vertical speed `(speed_x, speed_y)`.
  - **Color:** RGB values `(R, G, B)`.
  - **Dimensions:** Width and height of the rectangle.

---

## Requirements

1. Parse the configuration file to extract window properties, font settings, and figure details.
2. Create a graphical window using a library like **SFML**.
3. Render the specified figures with their initial properties.
4. Animate the figures based on their movement speed.
5. Display the specified label in the center of each figure.
6. Ensure that when a figure reaches an edge of the window, it reverses its movement direction, similar to the behavior of old idle DVD screen savers.

---

## Implementation Notes

- Ensure the program handles errors in the configuration file gracefully (e.g., missing or invalid values).
- Use appropriate data structures to store the parsed figures and properties.
- Incorporate clean and modular code to separate parsing, rendering, and animation logic.
- Allow flexibility for future enhancements, such as adding more shapes or effects.
