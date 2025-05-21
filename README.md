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
- **Database**: SQL 
- **Language**: C++

## 📌 Goals

- Smooth and user-friendly desktop interface
- Accurate worker-client matching based on availability, specialty, and location
- Efficient tracking of ratings and feedback
- Structured data storage with a relational SQL database

## 🏗️ How to Run

### 👤 Account Management
- If the client **does not have an account**, they can go to the **Signup** page and create one.
- If the client **already has an account**, they can go to the **Login** page and enter their credentials to sign in.

---

### 📄 Available Pages

#### 🧑‍🔧 Workers
- Displays a list of **available workers**, including:
  - Their **overall rating**
  - Their **specialties**
- When clicking on any worker, a window appears showing **all clients that the worker has worked with**.

#### 🛠️ Specialties
- Displays all **specialties** in the application.
- Shows the **best and worst worker** in each task based on **overall ratings**.

#### 📋 Tasks
- Displays a list of **available tasks** along with task **details**.
- When clicking on any task, you are forwarded to the **Add Request** page where you can:
  - **Add a new request** in this task
  - **Choose a worker**
  - **Select preferred time slots**
  - **Add an optional description**

#### 📨 Requests
- Displays all of the client's **requests** with their current **status**: 
  - `Pending`
  - `In Progress`
  - `Completed`
- When clicking on any request, you can:
  - **Edit** the request
  - **Delete** the request
  - **Mark it as completed**
    - Once marked as completed, a window appears allowing the client to **rate the worker** and **write optional feedback**

#### 🙍‍♂️ Profile
- Displays the **client's information**
- Allows the client to:
  - **Edit** their profile
  - **Delete** their account
  - Navigate to the **Requests** page that shows:
    - All **workers** who have worked with the client
    - Their **feedback** and **ratings**

---


## 📬 Contact
- Esraa Emary Abd El-Salam: [GitHub](https://github.com/esraa-emary) - [LinkedIn](https://www.linkedin.com/in/esraa-emary-b372b8303/)
- Adel ahmed Mohamed el hefny: [GitHub](https://github.com/AdelHefny) - [LinkedIn](https://www.linkedin.com/in/adel-hefny-75b6601bb?utm_source=share&utm_campaign=share_via&utm_content=profile&utm_medium=android_app)
- Mahmoud Abdelaziz: [GitHub](https://github.com/Mahmoudabdelaziz-2004) - [LinkedIn](https://www.linkedin.com/in/mahmoud-abdelaziz-240012347?utm_source=share&utm_campaign=share_via&utm_content=profile&utm_medium=android_app)
- Marwan Hussein: [GitHub](https://github.com/Marwan-Hussein) - [LinkedIn](www.linkedin.com/in/marawan-hussein-568373314)
- Andrew Wafae: [GitHub](https://github.com/20230069-Andrew) - [LinkedIn](https://www.linkedin.com/in/andrew-wafae-796a37358)

## 📜 License
This project is licensed under the MIT License.

