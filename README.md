# Task-Worker Matching Application

A desktop application that helps clients find and connect with suitable workers (e.g., plumbers, carpenters, electricians, etc.) for specific tasks. This system includes user registration, task management, rating and feedback mechanisms, and matching logic, all implemented with a GUI using **Qt C++** and backed by an **SQL database**.

## 💡 Features

### 👷 Worker Management
- Store worker details:
  - Unique ID
  - Name
  - One or more specialties
  - Supported work locations
  - Available time slots
- Calculate overall worker rating based on client feedback after completed tasks

### 🧑‍💼 Client Management
- Store client details:
  - Client ID
  - Name
  - Phone number
  - Address
  - Email address
  - Payment information
- Track overall feedback from workers about each client

### 🛠️ Task Management
- Store task details:
  - Task ID
  - Task name
  - Required specialty
  - Average time required
  - Average task fee

### 📋 Request Management
- Clients can submit task requests including:
  - Request ID
  - Client ID
  - Task ID
  - Request address
  - Time of request placement
  - Preferred time slot for task execution

### ✅ Execution Tracking
- After task execution, system records:
  - Request ID
  - Assigned worker ID
  - Actual time taken
  - Worker rating by client
  - Client rating by worker
  - Request status (e.g., open, finished)
  - Optional client and worker feedback

## 🖥️ Tech Stack

- **GUI Framework**: Qt (C++)
- **Database**: SQL (e.g., SQLite or MySQL)
- **Language**: C++

## 📌 Goals

- Smooth and user-friendly desktop interface
- Accurate worker-client matching based on availability, specialty, and location
- Efficient tracking of ratings and feedback
- Structured data storage with a relational SQL database

## 🏗️ How to Run (Coming Soon)

> Instructions for building and running the application will be added once development is underway.

## 📬 Contact
- Esraa Emary Abd El-Salam: [GitHub](https://github.com/esraa-emary) - [LinkedIn](https://www.linkedin.com/in/esraa-emary-b372b8303/)
- Adel ahmed Mohamed el hefny: [GitHub](https://github.com/AdelHefny) - [LinkedIn]()
- Mahmoud Abdelaziz: [GitHub]() - [LinkedIn]()
- Marwan Husseine: [GitHub]() - [LinkedIn]()
- Andrew Wafae: [GitHub]() - [LinkedIn]()

## 📜 License
This project is licensed under the MIT License.

