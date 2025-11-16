# Netlify Deployment Guide for Time Weaver

This guide will help you deploy the Time Weaver frontend to Netlify.

## Prerequisites

- A GitHub account
- A Netlify account (free at [netlify.com](https://www.netlify.com))
- Your backend deployed and accessible (Railway, Render, etc.)

## Step 1: Prepare Your Repository

1. **Initialize Git** (if not already done):
   ```bash
   cd "Time Weaver - Copy (5)"
   git init
   git add .
   git commit -m "Initial commit - ready for Netlify deployment"
   ```

2. **Create GitHub Repository**:
   - Go to [GitHub](https://github.com) and create a new repository
   - Name it something like `time-weaver` or `time-weaver-frontend`
   - Don't initialize with README (you already have files)

3. **Push to GitHub**:
   ```bash
   git remote add origin https://github.com/YOUR_USERNAME/YOUR_REPO_NAME.git
   git branch -M main
   git push -u origin main
   ```

## Step 2: Configure API URL

Before deploying, you need to set your backend API URL:

### Option A: Update Default URL in Code

Edit `public/app.js` line 12-20 and replace:
```javascript
const envApiUrl = window.API_BASE_URL || "https://your-backend-url.railway.app/api";
```
With your actual backend URL, for example:
```javascript
const envApiUrl = window.API_BASE_URL || "https://time-weaver-backend.railway.app/api";
```

### Option B: Use Netlify Environment Variable (Recommended)

1. After deploying, go to Netlify Dashboard
2. Site settings → Environment variables
3. Add new variable:
   - Key: `API_BASE_URL`
   - Value: `https://your-backend-url.railway.app/api`
4. Redeploy the site

## Step 3: Deploy to Netlify

### Method 1: Deploy via Netlify Dashboard (Easiest)

1. **Go to Netlify**:
   - Visit [app.netlify.com](https://app.netlify.com)
   - Sign up or log in (you can use GitHub to sign in)

2. **Import Your Project**:
   - Click "Add new site" → "Import an existing project"
   - Click "Deploy with GitHub"
   - Authorize Netlify to access your GitHub
   - Select your repository

3. **Configure Build Settings**:
   - **Base directory**: Leave empty (or set to root if your `public` folder is at root)
   - **Publish directory**: `public`
   - **Build command**: Leave empty (no build needed for static site)

4. **Deploy**:
   - Click "Deploy site"
   - Wait for deployment to complete (usually 1-2 minutes)
   - Your site will be live at `https://random-name-123.netlify.app`

5. **Set Custom Domain** (Optional):
   - Go to Site settings → Domain management
   - Add your custom domain

### Method 2: Deploy via Netlify CLI

1. **Install Netlify CLI**:
   ```bash
   npm install -g netlify-cli
   ```

2. **Login to Netlify**:
   ```bash
   netlify login
   ```

3. **Initialize and Deploy**:
   ```bash
   cd "Time Weaver - Copy (5)"
   netlify init
   # Follow prompts:
   # - Create & configure a new site
   # - Publish directory: public
   # - Build command: (leave empty)
   
   netlify deploy --prod
   ```

## Step 4: Configure Environment Variables (If Using Option B)

1. In Netlify Dashboard → Your site → Site settings
2. Go to "Environment variables"
3. Click "Add variable"
4. Add:
   - **Key**: `API_BASE_URL`
   - **Value**: `https://your-backend-url.railway.app/api`
5. Click "Save"
6. Go to "Deploys" tab and click "Trigger deploy" → "Clear cache and deploy site"

## Step 5: Update app.js to Use Environment Variable

If you're using environment variables, you need to inject them at build time. Update `public/app.js`:

Replace the API_BASE section with:
```javascript
const API_BASE = (() => {
  const hostname = window.location.hostname;
  if (hostname === 'localhost' || hostname === '127.0.0.1') {
    return "http://localhost:8080/api";
  }
  // Use environment variable if available, otherwise use default
  return window.API_BASE_URL || "https://your-backend-url.railway.app/api";
})();
```

Then create a script tag in `public/index.html` before the closing `</body>` tag:
```html
<script>
  // Inject Netlify environment variable
  window.API_BASE_URL = "%API_BASE_URL%";
</script>
```

And update `netlify.toml` to process the HTML:
```toml
[build]
  publish = "public"
  command = "echo 'Processing environment variables...'"

[build.processing]
  skip_processing = false

[build.processing.html]
  pretty_urls = true
```

Actually, a simpler approach: Create a small config file that gets loaded. But the easiest is to just hardcode your backend URL in app.js for now.

## Step 6: Test Your Deployment

1. **Visit your Netlify URL**: `https://your-site.netlify.app`
2. **Open Browser Console** (F12)
3. **Check for errors**:
   - Look for CORS errors (backend needs to allow your Netlify domain)
   - Look for 404 errors (API URL might be wrong)
   - Look for network errors (backend might be down)

4. **Test Features**:
   - Try logging in
   - Create an event
   - Test search functionality

## Troubleshooting

### CORS Errors

If you see CORS errors in the console, your backend needs to allow requests from your Netlify domain. 

In your `server.cpp`, make sure you have:
```cpp
response << "Access-Control-Allow-Origin: *\r\n";
```

Or better, allow specific origins:
```cpp
response << "Access-Control-Allow-Origin: https://your-site.netlify.app\r\n";
```

### API Not Found (404)

- Check that your backend URL is correct
- Make sure your backend is running and accessible
- Test the backend URL directly: `https://your-backend-url.railway.app/api/universities`

### Environment Variables Not Working

- Make sure you redeployed after adding environment variables
- Check that the variable name matches exactly
- For client-side JavaScript, you may need to use Netlify's build-time replacement

### Routing Issues (404 on refresh)

- Make sure `_redirects` file is in the `public` folder
- Check that `netlify.toml` has the redirect rule
- Redeploy after adding these files

## Quick Reference

- **Netlify Dashboard**: [app.netlify.com](https://app.netlify.com)
- **Your Site URL**: Check in Netlify dashboard after deployment
- **Build Logs**: Site → Deploys → Click on a deploy → See build logs
- **Function Logs**: If using Netlify Functions (not needed for this project)

## Next Steps

After deploying the frontend:
1. Deploy your backend to Railway or Render (see separate guide)
2. Update the API URL in your frontend code or environment variables
3. Test the full application
4. Set up a custom domain (optional)

## Support

If you encounter issues:
1. Check Netlify build logs
2. Check browser console for errors
3. Verify backend is accessible
4. Check CORS settings on backend

