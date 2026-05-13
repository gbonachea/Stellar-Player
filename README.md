# Stellar Player

Un reproductor de video moderno y elegante inspirado en Deepin Movie, construido con C++17, Qt 6 y libmpv.

## Características

- **Reproducción multimedia**: Soporte para archivos locales, streaming (HTTP, HTTPS, RTSP, RTMP, HLS) y arrastrar y soltar.
- **Controles completos**: Reproducir/pausar, detener, adelantar, retroceder, velocidad variable, bucle, volumen y mute.
- **Subtítulos**: Carga de archivos .srt, .ass, .ssa con ajustes de tamaño, color, posición y sincronización.
- **Listas de reproducción**: Soporte para M3U/M3U8 con reproducción aleatoria y repetición.
- **Pantalla**: Pantalla completa, always on top, relación de aspecto ajustable, rotación y captura de pantalla.
- **Interfaz moderna**: Tema oscuro, bordes redondeados, animaciones suaves y diseño minimalista.
- **Atajos de teclado**: Navegación completa con teclado.
- **Configuración persistente**: Guarda preferencias de usuario.

## Requisitos del sistema

- Linux (principal), Windows/macOS (compatibilidad opcional)
- Qt 6.2 o superior
- libmpv 0.33.0 o superior
- CMake 3.16 o superior
- Compilador C++17 (GCC 7+, Clang 5+, MSVC 2017+)

## Dependencias

### Ubuntu/Debian
```bash
sudo apt update
sudo apt install build-essential cmake qt6-base-dev libmpv-dev pkg-config
```

### Fedora
```bash
sudo dnf install cmake qt6-qtbase-devel mpv-devel pkgconfig
```

### Arch Linux
```bash
sudo pacman -S cmake qt6-base mpv pkgconf
```

### Windows (MSYS2)
```bash
pacman -S mingw-w64-x86_64-cmake mingw-w64-x86_64-qt6-base mingw-w64-x86_64-mpv
```

## Compilación

1. Clona o descarga el código fuente
2. Crea un directorio de compilación:
   ```bash
   mkdir build
   cd build
   ```
3. Configura con CMake:
   ```bash
   cmake ..
   ```
4. Compila:
   ```bash
   make -j$(nproc)
   ```
5. Instala (opcional):
   ```bash
   sudo make install
   ```

## Uso

### Ejecución
```bash
./StellarPlayer
```

### Atajos de teclado
- **Espacio**: Reproducir/pausar
- **F**: Pantalla completa
- **Esc**: Salir de pantalla completa
- **Flechas izquierda/derecha**: Retroceder/adelantar 5 segundos
- **Flechas arriba/abajo**: Ajustar volumen
- **M**: Mute
- **O**: Abrir archivo
- **S**: Cargar subtítulos

### Formatos soportados
- **Video**: MP4, AVI, MKV, MOV, WMV, FLV, WebM, etc.
- **Audio**: MP3, WAV, FLAC, AAC, OGG, etc.
- **Streaming**: HTTP, HTTPS, RTSP, RTMP, HLS (.m3u8)
- **Subtítulos**: SRT, ASS, SSA

## Arquitectura

```
src/
├── main.cpp                 # Punto de entrada
├── MainWindow.cpp           # Ventana principal y controles
├── MpvWidget.cpp            # Integración con libmpv
├── PlayerController.cpp     # Lógica de control del reproductor
├── PlaylistModel.cpp        # Modelo de lista de reproducción
├── SubtitleSettingsDialog.cpp # Diálogo de configuración de subtítulos
├── SettingsManager.cpp      # Gestión de configuración persistente
└── ThemeManager.cpp         # Gestión de temas

include/                     # Encabezados
resources/                   # Recursos Qt (.ui, .qrc)
styles/                      # Hojas de estilo QSS
translations/                # Archivos de traducción
```

## Contribución

1. Fork el proyecto
2. Crea una rama para tu feature (`git checkout -b feature/AmazingFeature`)
3. Commit tus cambios (`git commit -m 'Add some AmazingFeature'`)
4. Push a la rama (`git push origin feature/AmazingFeature`)
5. Abre un Pull Request

## Licencia

Este proyecto está bajo la Licencia MIT. Ver el archivo `LICENSE` para más detalles.

## Créditos

- Inspirado en [Deepin Movie](https://github.com/linuxdeepin/deepin-movie)
- Motor multimedia: [libmpv](https://mpv.io/)
- Framework GUI: [Qt](https://www.qt.io/)

## Soporte

Para reportar bugs o solicitar features, por favor abre un issue en GitHub.

---

**Stellar Player** - Un reproductor moderno para la era digital.