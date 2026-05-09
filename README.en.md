# Club Activity Management System

[中文](./README.md) | English | [日本語](./README.ja.md)

Club Activity Management System is a C language course-design project. It uses the Mongoose embedded HTTP server for backend APIs, HTML/CSS/JavaScript for frontend pages, and local text files for student accounts, activity records, and deleted-activity backups.

The project focuses on building a runnable B/S-style management system with minimal dependencies. Students can register, log in, browse activities, and enroll. Administrators can publish, edit, delete activities, view enrolled students, and inspect statistics.

## Features

### Student Side

- Student ID registration and login.
- Browse activity list.
- Filter or view activities by status and category.
- Enroll in activities.
- Check enrollment state.

### Admin Side

- Administrator login.
- Publish activities with name, category, location, capacity, and deadline.
- Edit activity information.
- Delete activities and write backup records.
- View enrolled students.
- View activity statistics.

### Backend Validation

- Student ID format validation.
- Reserved administrator account.
- Activity date format validation.
- Capacity limit checks.
- Duplicate enrollment checks.
- Expired activity state update.

## Stack

- C
- Mongoose HTTP library
- HTML / CSS / JavaScript
- Fetch API
- Local TXT file persistence

## Quick Start

### Compile

```bash
gcc main.c mongoose.c -o activity-system
```

If your platform requires additional networking libraries, add the corresponding linker flags for your environment.

### Run

```bash
./activity-system
```

Then open:

```text
http://localhost:8000
```

The default administrator account is defined in `main.c`:

```text
username: admin
password: admin123
```

## Project Structure

```text
.
├── Data/
│   ├── activity.txt
│   ├── student.txt
│   └── trash.txt
├── frontend/
│   ├── admin.html
│   ├── index.html
│   └── student.html
├── Doc.md
├── main.c
├── mongoose.c
└── mongoose.h
```

| Path | Purpose |
| --- | --- |
| `main.c` | HTTP routes, business logic, linked-list data structures, and file IO |
| `mongoose.c` / `mongoose.h` | Embedded HTTP server library |
| `frontend/index.html` | Login and registration entry |
| `frontend/student.html` | Student activity browsing and enrollment page |
| `frontend/admin.html` | Administrator dashboard |
| `Data/activity.txt` | Activities and enrolled students |
| `Data/student.txt` | Student account data |
| `Data/trash.txt` | Deleted activity backup records |
| `Doc.md` | Course-design report and module explanation |

## Data Format

Activities are stored in `Data/activity.txt`. Each line is one activity record:

```text
ID Name Category Location Capacity Status Deadline StudentID1|StudentID2|...
```

Students are stored in `Data/student.txt`. Each line contains student ID, name, password, phone number, and class.

## API Design

The backend listens on `http://localhost:8000` and routes requests by URI. Main endpoints include:

| Endpoint | Description |
| --- | --- |
| `/api/login` | Login |
| `/api/register` | Registration |
| `/api/add` | Admin creates an activity |
| `/api/change` | Admin edits an activity |
| `/api/delete` | Admin deletes an activity |
| `/api/enroll` | Student enrolls in an activity |
| `/api/list` | Fetch activity list |

Requests that do not match an API route fall back to static files under `frontend/`.

## Design Notes

The project keeps students and activities in linked lists in memory. It loads data from `Data/` on startup and writes changes back to text files after operations. This keeps the implementation simple and readable, which is appropriate for demonstrating data structures, file persistence, and HTTP interaction in a course-design context.

The project does not include a database, session system, or production-grade permission model. It is best treated as a C Web practice project, a Mongoose example, and course-design material.

## License

This repository does not declare an open-source license yet. Confirm authorization before reuse, redistribution, or using it as course-submission material.
