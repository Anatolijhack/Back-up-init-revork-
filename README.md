# 🗄️ Backup System (C++)

Многопоточная система резервного копирования с **pipeline-архитектурой**, ориентированная на обработку больших объёмов данных.

> Проект демонстрирует подход **dataflow + concurrency**, используемый в production-системах.

---

## 🚀 Возможности

* 📂 Рекурсивное копирование директорий
* ⚡ Многопоточность (ThreadPool)
* 🔗 Pipeline-архитектура обработки файлов
* 🔁 Incremental backup (только изменённые файлы)
* 🔄 Sync режим (удаление лишних файлов)
* 🧪 Dry-run (без реальных изменений)
* 🧾 Потокобезопасное логирование
* 🧩 CLI через Command Pattern

---

# 🔗 Pipeline Architecture

Система построена как **цепочка этапов обработки данных**, где каждый файл проходит через pipeline.

---

## 📦 Pipeline обработки

```text id="wq91zc"
[Scan Files]
      ↓
[Filter Stage]
 (оставляем только regular files)
      ↓
[Prepare Stage]
 (src → dst пути)
      ↓
[Copy Stage]
 (копирование файла)
      ↓
[Log Stage]
 (логирование результата)
```

---

## ⚙️ Реализация pipeline

```cpp id="g3y7kp"
connect(filterStage, prepareStage);
connect(prepareStage, copyStage);
connect(copyStage, logStage);
```

Каждый этап представлен как:

```cpp id="2l6t7m"
Stage<In, Out>
```

---

## 🧠 Как это работает

1. **Scan** — проходит по файловой системе
2. **Filter** — отбрасывает всё, кроме файлов
3. **Prepare** — вычисляет путь назначения
4. **Copy** — выполняет копирование
5. **Log** — фиксирует результат

---

## 🧵 Параллельность

* Каждый stage выполняется асинхронно
* Используется `ThreadPool`
* Файлы обрабатываются **параллельно**
* Pipeline позволяет держать CPU загруженным

---

## ⚡ Поток выполнения

```text id="3z7lqv"
File1 ──► Filter ──► Prepare ──► Copy ──► Log
File2 ──► Filter ──► Prepare ──► Copy ──► Log
File3 ──► Filter ──► Prepare ──► Copy ──► Log
```

---

## ⚙️ Использование

### 🔹 Прямой запуск

```cpp id="k9m2pz"
Config cfg;
cfg.sources = "C:/data";
cfg.destiantion = "Backup";
cfg.threads = 4;

BackupManager manager(cfg);
manager.run();
```

---

### 🔹 CLI

```bash id="9n4kxt"
backup run --source=PATH --dest=PATH [options]
```

Пример:

```bash id="p8c4sl"
backup run --source=C:/data --dest=D:/backup --incremental --sync
```

---

## 🔧 Опции

| Опция           | Описание                           |
| --------------- | ---------------------------------- |
| `--incremental` | копировать только изменённые файлы |
| `--sync`        | удалять лишние файлы               |
| `--dry-run`     | показать действия без выполнения   |

---

## 🧾 Логирование

Файл:

```text id="0i2d9r"
backup.log
```

Пример:

```text id="m2z4qk"
[INFO] Copied: file.txt
[WARN] Deleted: old_file.txt
[ERROR] Failed to open file
```

---

## 📁 Архитектура

* **BackupManager** — управление процессом
* **Stage<In, Out>** — этап pipeline
* **ThreadPool** — выполнение задач
* **Logger** — потокобезопасный лог
* **CommandDispatcher** — CLI
* **Commands** — run / init / status

---

## 🎯 Что демонстрирует проект

* Pipeline / Dataflow архитектуру
* Многопоточность и синхронизацию
* Разделение ответственности (SRP)
* Обработку файловой системы
* System design подход

---

## ⚠️ Ограничения

* Упрощённый file hash
* Нет централизованной обработки ошибок
* Нет graceful shutdown pipeline
* Ограниченная cross-platform поддержка

---

## 📈 Roadmap

* [ ] Улучшение hash (CRC32 / SHA)
* [ ] Обработка ошибок в pipeline
* [ ] Завершение pipeline (graceful stop)
* [ ] Поддержка config файла
* [ ] Оптимизация ThreadPool

---

## 🧑‍💻 Автор

Pet-проект с фокусом на:

> **C++ / Concurrency / System Design**

---

## 💡 Итог

Проект показывает, как строить:

* масштабируемые системы обработки
* многопоточные pipeline
* архитектуру, близкую к production решениям
