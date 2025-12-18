# SimpleMp3Player (Qt6)

Невеликий MP3-плеєр на **Qt 6** (Widgets + Multimedia).

## Вимоги

- **Qt 6** з модулями:
  - `Qt6 Widgets`
  - `Qt6 Multimedia`
- **CMake** (встановлено через winget)
- Для цього репозиторію налаштовано під **Qt 6.10.1 MinGW**:
  - Qt: `D:\Qt\6.10.1\mingw_64`
  - MinGW: `D:\Qt\Tools\mingw1310_64\bin`

> Якщо твій Qt встановлений в інше місце або інша версія — онови шляхи у VS Code tasks або у команді CMake.

## Запуск у VS Code (рекомендовано)

У проєкті вже є tasks у [\.vscode/tasks.json](.vscode/tasks.json).

1. `Ctrl+Shift+P` → `Tasks: Run Task`
2. Запусти по черзі:
   - **mp3: configure (Qt6 MinGW)**
   - **mp3: build (Qt6 MinGW)**
   - **mp3: run (Qt6 MinGW)**

`run` запускає застосунок через `cmd /c start`, щоб GUI-вікно відкривалось незалежно від термінала.

## Збірка/запуск з терміналу (PowerShell)

Спочатку додай Qt/MinGW у `PATH` (важливо для DLL):

```powershell
$env:Path = 'D:\Qt\Tools\mingw1310_64\bin;D:\Qt\6.10.1\mingw_64\bin;' + $env:Path
```

Конфігурація (один раз або після змін CMakeLists.txt):

```powershell
& "C:\Program Files\CMake\bin\cmake.exe" -S "C:\Users\User\Documents\programming\cpp\mp3" -B "C:\Users\User\Documents\programming\cpp\mp3\build-mingw" -G "MinGW Makefiles" `
  -DCMAKE_PREFIX_PATH="D:/Qt/6.10.1/mingw_64" `
  -DCMAKE_MAKE_PROGRAM="D:/Qt/Tools/mingw1310_64/bin/mingw32-make.exe" `
  -DCMAKE_CXX_COMPILER="D:/Qt/Tools/mingw1310_64/bin/g++.exe"
```

Збірка:

```powershell
& "C:\Program Files\CMake\bin\cmake.exe" --build "C:\Users\User\Documents\programming\cpp\mp3\build-mingw" -j 8
```

Запуск:

```powershell
& "C:\Users\User\Documents\programming\cpp\mp3\build-mingw\SimpleMp3Player.exe"
```

## Як користуватись

- **Додати MP3** — вибери `.mp3`, трек додасться у список і стане поточним.
- **Play / Pause** — відтворення / пауза.
- **Stop** — зупинка.
- **Слайдер** — перемотування.
- **Фон** — вибір картинки фону.

Фон зберігається через `QSettings` і відновлюється при наступному запуску.

## Типові проблеми

### 1) `cmake` не знаходиться у VS Code tasks

Якщо tasks пишуть `cmake is not recognized`, у нас це вже вирішено тим, що tasks викликають:

- `C:\Program Files\CMake\bin\cmake.exe`

Якщо CMake встановлений в інше місце — онови шлях у [\.vscode/tasks.json](.vscode/tasks.json).

### 2) Не запускається EXE / бракує DLL

Зазвичай причина — немає Qt/MinGW у `PATH`.

У tasks це вже прописано через `options.env.Path`. Якщо запускаєш руками — додай у `PATH`:

- `D:\Qt\Tools\mingw1310_64\bin`
- `D:\Qt\6.10.1\mingw_64\bin`

### 3) Фон не видно

Фон застосовується до `centralWidget()`; список треків зроблений прозорим, щоб фон був видимий.

## Структура

- `main.cpp` — старт застосунку (`QApplication`).
- `MainWindow.h/.cpp` — UI та логіка плеєра.
- `CMakeLists.txt` — збірка через CMake + Qt.
