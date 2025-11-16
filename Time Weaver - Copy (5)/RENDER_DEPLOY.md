# Deploy to Render - Step by Step Guide

## ✅ Yes, Your App Will Run on Render!

Your Flask app is **100% ready** for Render. Just follow these steps:

## 🚀 Quick Deployment Steps

### Step 1: Push to GitHub
1. Create a new repository on GitHub
2. Push all your files to GitHub:
   ```bash
   git init
   git add .
   git commit -m "Initial commit"
   git remote add origin https://github.com/YOUR_USERNAME/YOUR_REPO.git
   git push -u origin main
   ```

### Step 2: Deploy on Render
1. Go to [render.com](https://render.com)
2. Sign up (free) - use GitHub to sign in
3. Click **"New +"** → **"Web Service"**
4. Connect your GitHub repository
5. Render will auto-detect it's a Python app
6. Configure:
   - **Name:** `time-weaver` (or any name you want)
   - **Environment:** `Python 3`
   - **Build Command:** `pip install -r requirements.txt`
   - **Start Command:** `python app.py`
   - **Plan:** Free (or choose paid if needed)

7. Click **"Create Web Service"**

### Step 3: Wait for Deployment
- Render will install dependencies
- Build your app
- Start the server
- Give you a URL like: `https://time-weaver.onrender.com`

**That's it! Your app is live! 🎉**

## 📋 What Render Needs (Already Configured)

✅ **requirements.txt** - ✅ Present  
✅ **app.py** - ✅ Present  
✅ **Start command** - ✅ `python app.py`  
✅ **Port configuration** - ✅ Uses `PORT` environment variable  
✅ **Static files** - ✅ Served from `public/` folder  

## 🔧 Optional: Use render.yaml

I've created a `render.yaml` file that makes deployment even easier:
- Just select "Apply render.yaml" when creating the service
- All settings are pre-configured!

## ⚠️ Important Notes

### Database
- SQLite (`calendar.db`) will be created automatically
- **Note:** On Render's free tier, the database resets when the app sleeps
- For production, consider using Render's PostgreSQL (free tier available)

### Environment Variables
- `PORT` - Automatically set by Render (don't change)
- `PYTHON_VERSION` - Set to 3.11

### Free Tier Limitations
- App sleeps after 15 minutes of inactivity
- First request after sleep takes ~30 seconds (cold start)
- Database resets on sleep (on free tier)

## 🎯 Testing After Deployment

1. Visit your Render URL
2. Try logging in with:
   - Username: `admin`
   - Password: `admin123`
3. Create a test event
4. Everything should work!

## 🐛 Troubleshooting

**Build fails?**
- Check Render logs
- Make sure `requirements.txt` is correct
- Verify Python version (3.11)

**App won't start?**
- Check logs for errors
- Verify `app.py` is in root directory
- Make sure port is set correctly (Render sets it automatically)

**Database errors?**
- Database is created on first run
- Check file permissions in logs

## 📊 Render Dashboard

After deployment, you can:
- View logs in real-time
- See metrics (CPU, memory)
- Restart the service
- View environment variables

## ✅ Your App is Ready!

Everything is configured correctly. Just push to GitHub and deploy on Render!

