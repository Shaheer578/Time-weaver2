# Render.com Deployment Guide for Time Weaver Backend

Since Railway's free tier only supports databases, we'll use **Render.com** which offers free web service hosting.

## Why Render.com?

✅ **Free tier available** for web services  
✅ **Supports Docker** (perfect for C++ applications)  
✅ **Automatic deployments** from GitHub  
✅ **Persistent storage** for SQLite database  
✅ **HTTPS included**  

⚠️ **Note**: Free tier services spin down after 15 minutes of inactivity, but wake up automatically on first request (may take ~30 seconds).

## Prerequisites

- GitHub account
- Render.com account (free at [render.com](https://render.com))
- Your code pushed to GitHub

## Step 1: Prepare Your Code

The following files have been created/updated:
- ✅ `Dockerfile` - Container configuration
- ✅ `render.yaml` - Render deployment config
- ✅ `server.cpp` - Updated to use PORT environment variable
- ✅ `.dockerignore` - Excludes unnecessary files

## Step 2: Push to GitHub

If you haven't already:

```bash
cd "Time Weaver - Copy (5)"
git add .
git commit -m "Add Render.com deployment configuration"
git push
```

## Step 3: Deploy to Render.com

### Method 1: Using render.yaml (Recommended)

1. **Go to Render Dashboard**:
   - Visit [dashboard.render.com](https://dashboard.render.com)
   - Sign up or log in (you can use GitHub to sign in)

2. **Create New Web Service**:
   - Click "New +" → "Web Service"
   - Click "Build and deploy from a Git repository"
   - Connect your GitHub account if not already connected
   - Select your repository

3. **Configure Service**:
   - **Name**: `time-weaver-backend` (or any name you prefer)
   - **Region**: Choose closest to you (e.g., `Oregon (US West)`)
   - **Branch**: `main` (or your default branch)
   - **Root Directory**: Leave empty (or `.` if needed)
   - **Environment**: `Docker`
   - **Dockerfile Path**: `./Dockerfile` (should auto-detect)
   - **Docker Context**: `.` (current directory)

4. **Environment Variables**:
   - Render will automatically set `PORT=8080`
   - You can add more if needed later

5. **Plan**:
   - Select **Free** (for testing)
   - Or **Starter** ($7/month) if you need it always running

6. **Deploy**:
   - Click "Create Web Service"
   - Render will start building your Docker image
   - This takes 5-10 minutes the first time

### Method 2: Manual Configuration

If render.yaml doesn't work:

1. **New Web Service** → Connect GitHub repo
2. **Settings**:
   - **Name**: `time-weaver-backend`
   - **Environment**: `Docker`
   - **Dockerfile Path**: `./Dockerfile`
   - **Docker Context**: `.`
   - **Build Command**: (leave empty, Dockerfile handles it)
   - **Start Command**: (leave empty, Dockerfile handles it)
3. **Environment Variables**:
   - Add: `PORT` = `8080`
4. **Plan**: Free
5. **Deploy**

## Step 4: Get Your Backend URL

After deployment completes:

1. Your service will have a URL like: `https://time-weaver-backend.onrender.com`
2. **Test it**: Visit `https://time-weaver-backend.onrender.com/api/universities`
3. You should see JSON with universities list

## Step 5: Update Frontend API URL

Now update your Netlify frontend to use this backend:

1. **Edit `public/app.js`**:
   - Find line 22
   - Replace `"https://your-backend-url.railway.app/api"` 
   - With: `"https://time-weaver-backend.onrender.com/api"` (or your actual Render URL)

2. **Redeploy Netlify**:
   - Push the change to GitHub
   - Netlify will auto-deploy
   - Or manually trigger deploy in Netlify dashboard

## Step 6: Configure CORS (If Needed)

Your `server.cpp` already has CORS headers:
```cpp
response << "Access-Control-Allow-Origin: *\r\n";
```

This allows all origins. For production, you might want to restrict it to your Netlify domain:

```cpp
response << "Access-Control-Allow-Origin: https://your-site.netlify.app\r\n";
```

## Troubleshooting

### Build Fails

**Error**: "g++: command not found"
- **Solution**: Dockerfile should install g++, check that it's correct

**Error**: "sqlite3.h: No such file"
- **Solution**: Make sure `libsqlite3-dev` is installed in Dockerfile (it is)

**Error**: "bind failed" or "port already in use"
- **Solution**: Make sure server.cpp uses PORT env var (already updated)

### Service Won't Start

1. **Check Logs**:
   - In Render dashboard → Your service → Logs
   - Look for error messages

2. **Common Issues**:
   - Database file permissions (SQLite should create it automatically)
   - Missing files (check .dockerignore)
   - Port binding issues (should be fixed with PORT env var)

### Service Spins Down (Free Tier)

- **Issue**: After 15 min inactivity, service sleeps
- **Solution**: 
  - First request takes ~30 seconds to wake up
  - Upgrade to Starter plan ($7/month) for always-on
  - Or use a free uptime monitor to ping it every 10 minutes

### Database Persistence

- **Issue**: Database resets on redeploy
- **Solution**: 
  - Render free tier includes persistent disk
  - Database file should persist between deployments
  - If not, consider using Render PostgreSQL (free tier available)

## Testing Your Deployment

1. **Test Backend**:
   ```bash
   curl https://your-service.onrender.com/api/universities
   ```

2. **Test from Frontend**:
   - Open your Netlify site
   - Open browser console (F12)
   - Try logging in
   - Check for API errors

3. **Check Logs**:
   - Render dashboard → Logs
   - See real-time server logs

## Alternative: Fly.io (Another Free Option)

If Render doesn't work, try **Fly.io**:

1. Install Fly CLI: `curl -L https://fly.io/install.sh | sh`
2. Login: `fly auth login`
3. Launch: `fly launch` (in your project directory)
4. Follow prompts

Fly.io also has a free tier with better performance than Render's free tier.

## Cost Comparison

| Platform | Free Tier | Paid Tier |
|----------|-----------|-----------|
| **Render** | ✅ (spins down) | $7/month (always on) |
| **Fly.io** | ✅ (limited resources) | $1.94/month+ |
| **Railway** | ❌ (databases only) | $5/month+ |

## Next Steps

1. ✅ Deploy backend to Render
2. ✅ Get backend URL
3. ✅ Update frontend API URL
4. ✅ Test full application
5. ✅ Set up custom domain (optional)

## Support

- **Render Docs**: [render.com/docs](https://render.com/docs)
- **Render Status**: [status.render.com](https://status.render.com)
- **Community**: [community.render.com](https://community.render.com)

