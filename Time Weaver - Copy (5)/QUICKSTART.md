# Quick Start Guide - Time Weaver

## 🚀 Quick Local Setup

1. **Install Python 3.11+** (if not already installed)

2. **Install dependencies:**
```bash
pip install -r requirements.txt
```

3. **Run the application:**
```bash
python app.py
```

4. **Open in browser:**
```
http://localhost:8080
```

That's it! The database will be created automatically.

## 📦 What Was Converted

- ✅ `server.cpp` → `app.py` (Flask application)
- ✅ `database.cpp` → `database.py` (SQLite database manager)
- ✅ `dsa_structures.h` → `dsa_structures.py` (Data structures)
- ✅ All API endpoints preserved
- ✅ All functionality maintained

## 🌐 Deployment Options

### Option 1: Railway (Easiest)
1. Sign up at [railway.app](https://railway.app)
2. Connect GitHub repository
3. Deploy automatically!

### Option 2: Render
1. Sign up at [render.com](https://render.com)
2. Create Web Service
3. Connect repository
4. Set start command: `python app.py`

### Option 3: Vercel
1. Install Vercel CLI: `npm i -g vercel`
2. Run: `vercel`
3. Follow prompts

## 🔑 Default Login

- **Admin**: username: `admin`, password: `admin123`

## 📝 Notes

- Database file (`calendar.db`) is created automatically
- Holidays are loaded from `HolidayData.csv`
- All original features are preserved

## 🐛 Troubleshooting

**Port already in use?**
- Change port in `app.py`: `app.run(port=8081)`

**Database errors?**
- Delete `calendar.db` and restart (will recreate)

**Import errors?**
- Make sure you're in the project root directory
- Check Python version: `python --version` (should be 3.11+)

## 📚 Full Documentation

See `README.md` and `DEPLOYMENT.md` for more details.

