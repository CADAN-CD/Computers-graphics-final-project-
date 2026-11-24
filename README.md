# Computers-graphics-final-project-
# Room: Diorama 3D con OpenGL

Proyecto de gráficos por computadora hecho con **C++ y OpenGL 3.3 (core)**. Carga un diorama en formato glTF/GLB, lo ilumina con una luz puntual que sale de un cristal flotante, y permite mover ese cristal a mano o a lo largo de una **curva de Bézier**, todo desde una interfaz con ImGui.

## Características

- Cargador propio de modelos **`.gltf` (+ `.bin`) y `.glb`**: recorre los nodos de la escena, aplica sus transformaciones y lee posiciones, normales, UVs e índices.
- **Texturas** desde archivo o embebidas en el GLB (`stb_image`), y color base por material (`baseColorFactor`).
- **Skybox** con cubemap.
- **Luz puntual** (ambiente + difusa + atenuación) cuya posición es la del cristal, así que el diorama se ilumina según donde esté.
- **Cristal** dibujado con un shader sin iluminación, tintado con el color de la luz.
- **Curva de Bézier cúbica** (clase `BezierPath`) para animar el cristal.
- **Panel ImGui** para controlar el cristal, la luz y la curva en tiempo real.
- Cámara libre con teclado y ratón.

## Tecnologías

| Librerías- Uso 

 GLFW - Ventana y entrada 
 GLAD - Carga de funciones de OpenGL 
 GLM - Matemáticas (vectores, matrices, cuaterniones) 
 stb_image - Carga de imágenes y texturas 
 nlohmann/json - Lectura del JSON de glTF/GLB 
 Dear ImGui - Interfaz de controles 

## Estructura del proyecto

```
main.cpp              Bucle principal, ImGui, orden de dibujado
Model.h/.cpp          Cargador glTF/GLB (nodos, buffers, materiales, texturas)
Mesh.h/.cpp           Un mesh: VAO + texturas + color base
VAO / VBO / EBO       Envoltorios de los objetos de OpenGL
Texture.h/.cpp        Texturas desde archivo o memoria
shaderClass.h/.cpp    Compilación y uso de shaders
Camera.h/.cpp         Cámara, matriz de vista/proyección, dibujo del skybox
KeyboardInputs.h/.cpp Movimiento de la cámara
Skybox.h/.cpp         Cubemap
BezierPath.h/.cpp     Curva de Bézier cúbica
default.vert/.frag    Shader del diorama (con luz puntual)
light.vert/.frag      Shader del cristal (sin iluminación)
skybox.vert/.frag     Shader del skybox
Models/               scene.gltf, scene.bin, purple_crystal.glb, ...
SkyBox/               px, nx, py, ny, pz, nz (.png)
```


## Cómo funciona la iluminación

El cristal es la fuente de luz. Cada frame se envía al shader del diorama:

```
pointLightPos   = posición del cristal
pointLightColor = lightColor * lightIntensity
```

`default.frag` calcula `ambiente + difusa * atenuación`, donde la atenuación es `1 / (constant + linear*d + quadratic*d²)`. Como diorama y cristal leen la misma variable (`crystalPosition`), la luz siempre coincide con el modelo.

## Curva de Bézier

`BezierPath` guarda 4 puntos de control y evalúa:

```
B(t) = (1-t)³·P0 + 3(1-t)²t·P1 + 3(1-t)t²·P2 + t³·P3,   t en [0, 1]
```

| Miembro | Función |
|---|---|
| `animate` | Activa el movimiento automático |
| `pingPong` | Ida y vuelta (`true`) o reinicio al llegar (`false`) |
| `speed` | Fracción de curva recorrida por segundo |
| `t` | Posición actual en la curva |
| `update(dt)` | Avanza `t` |
| `getPosition()` | Punto actual de la curva |
| `getTangent()` | Dirección de movimiento (normalizada) |
| `sample(n)` | Puntos para dibujar la curva |

## Controles

- **Cámara:** teclado y ratón :
(wasd) para movimientos basicos en pantalla .
(u-i) arriba y abajo respectivamente
()
- **Panel "Crystal Controls" (ImGui):**
  - Posición, rotación y escala del cristal
  - Color e intensidad de la luz
  - Animación de Bézier: `Animate`, `Ping-pong`, `Speed`, `t` y los puntos `P0` a `P3`


## Créditos

Modelos 3D y texturas: 
 `scene.gltf` 
, `purple_crystal.glb` - iPoly3D
 skybox-Screaming Brain Studios
