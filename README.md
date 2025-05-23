
# Minimalistic REST API Messenger

A lightweight and modular REST API-based messaging backend written in modern C++20. This server provides secure, TLS-encrypted endpoints for user authentication, group creation, and message exchange. Ideal for experimentation, extension, or as a microservice in a larger architecture.

---

## ✨ Features

* ⚙️ Modular structure: clearly separated networking, database, crypto, and core logic
* 🔐 TLS-secured communication with OpenSSL
* 🧾 Lightweight SQLite backend for persistence
* 📬 RESTful endpoints for user auth, group chat, and messaging
* 🚦 Robust error handling with status-aware responses
* 📦 Boost libraries for JSON, threading, and networking

---

## 📁 Directory Structure

```
restAPI/
├── CMakeLists.txt         # CMake build configuration
├── main.cpp               # Server entry point
├── network_handler/       # HTTP server, routing, and TLS setup
├── db/                    # SQLite DB access and models
├── crypto/                # Cryptographic logic (e.g., GUIDs, hashing)
├── src/                   # REST API request handlers (core logic)
├── tests/                 # Unit/integration tests
```

---

## 🚀 Quickstart

### 🔧 Prerequisites

* **C++20 compiler**
* **CMake ≥ 3.31**
* **Boost ≥ 1.66** (`system`, `thread`, `json`)
* **OpenSSL**
* **SQLite3 development libraries**

### 🔨 Build Instructions

```bash
git clone https://github.com/your-username/restAPI.git
cd restAPI
mkdir build && cd build
cmake ..
make
```

### ▶️ Run

```bash
./restAPI
```

**Note**: Ensure you update the TLS certificate paths in `main.cpp`:

```cpp
std::string cert = "/absolute/path/to/server.crt";
std::string key  = "/absolute/path/to/server.key";
```

---

## 📡 API Reference

### 🔁 Health Check

```http
GET /
```

**Response**:

```json
{"status": "up"}
```

---

### 👤 User Registration

```http
POST /signup
Content-Type: application/json
```

**Body:**

```json
{
  "username": "alice",
  "password": "securepass"
}
```

---

### 🔑 User Login (Get Auth Token)

```http
GET /token
Content-Type: application/json
```

**Body:**

```json
{
  "username": "alice",
  "password": "securepass"
}
```

**Response:**

```json
{
  "status": "OK",
  "guid": "user-session-token"
}
```

---

### 👥 Create Group

```http
POST /group
Content-Type: application/json
```

**Body:**

```json
{
  "username": "alice",
  "guid": "user-session-token",
  "name": "project-team",
  "members": ["alice", "bob", "carol"]
}
```

---

### 📨 Send Message

```http
POST /message
Content-Type: application/json
```

**Body:**

```json
{
  "username": "alice",
  "guid": "user-session-token",
  "group_id": 1,
  "content": "Hello, team!"
}
```

---

### 📬 Get Group List

```http
GET /group
Content-Type: application/json
```

**Body:**

```json
{
  "username": "alice",
  "guid": "user-session-token"
}
```

**Response:**

```json
{
  "status": "OK",
  "group_ids": [1, 2, 3]
}
```

---

### ℹ️ Get Group Info

```http
GET /group/info
Content-Type: application/json
```

**Body:**

```json
{
  "username": "alice",
  "guid": "user-session-token",
  "group_id": 1
}
```

**Response:**

```json
{
  "status": "OK",
  "name": "project-team",
  "members": ["alice", "bob", "carol"]
}
```

---

## 🧪 Testing

```bash
cd build
make tests
./tests/test_binary_name  # Replace with your test binary name
```

---

## 🛡️ Security

* All requests are encrypted via HTTPS (TLS)
* GUID-based session tokens for authentication
* Thread-safe login and group logic using shared mutexes

---

## 📄 License

MIT License. See [`LICENSE`](./LICENSE) for full text.

---

## 🙌 Contributions

Contributions and improvements are welcome! Feel free to open issues, suggest improvements, or submit pull requests.

---
