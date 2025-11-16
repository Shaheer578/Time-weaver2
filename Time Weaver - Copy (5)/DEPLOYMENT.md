# Deployment Guide - Time Weaver

## Netlify Deployment

### Important Note
Netlify primarily supports serverless functions, not full Flask applications. For this project, you have several options:

### Option 1: Use Railway or Render (Recommended for Flask)

**Railway:**
1. Sign up at [railway.app](https://railway.app)
2. Create new project from GitHub
3. Railway will auto-detect Python
4. Set start command: `python app.py`
5. Deploy!

**Render:**
1. Sign up at [render.com](https://render.com)
2. Create new Web Service
3. Connect GitHub repository
4. Set:
   - Build Command: `pip install -r requirements.txt`
   - Start Command: `python app.py`
5. Deploy!

### Option 2: Convert to Netlify Serverless Functions

To use Netlify, you'll need to convert each Flask route to a separate serverless function. This is more work but provides better Netlify integration.

### Option 3: Use Vercel

Vercel has better Python support:

1. Install Vercel CLI: `npm i -g vercel`
2. Create `vercel.json`:
```json
{
  "version": 2,
  "builds": [
    {
      "src": "app.py",
      "use": "@vercel/python"
    }
  ],
  "routes": [
    {
      "src": "/(.*)",
      "dest": "app.py"
    }
  ]
}
```

3. Deploy: `vercel`

## Local Testing

1. Install dependencies:
```bash
pip install -r requirements.txt
```

2. Run the server:
```bash
python app.py
```

3. Access at `http://localhost:8080`

## Environment Variables

For production, set these environment variables:

- `PORT`: Server port (default: 8080)
- `DATABASE_URL`: If using external database (optional)

## Database Considerations

**Important**: SQLite files are ephemeral on serverless platforms. For production:

1. **Use external database:**
   - PostgreSQL (Supabase, Neon, etc.)
   - MySQL (PlanetScale, etc.)
   - Or any SQL database

2. **Update `database.py`:**
   - Replace SQLite connection with your database
   - Update connection string from environment variable

3. **Example with PostgreSQL:**
```python
import psycopg2
import os

def open_database(self):
    DATABASE_URL = os.environ.get('DATABASE_URL')
    self.db = psycopg2.connect(DATABASE_URL)
    return True
```

## File Structure for Deployment

```
.
├── app.py                    # Main Flask app
├── database.py              # Database manager
├── dsa_structures.py        # Data structures
├── requirements.txt         # Dependencies
├── runtime.txt             # Python version
├── HolidayData.csv         # Holiday data
└── public/                 # Frontend (served as static)
    ├── index.html
    ├── app.js
    └── style.css
```

## Build Commands

**Railway/Render:**
- Build: `pip install -r requirements.txt`
- Start: `python app.py`

**Vercel:**
- Auto-detected (no build command needed)

**Netlify:**
- Build: `pip install -r requirements.txt`
- Publish: `public` (static files only)
- Functions: `.netlify/functions/` (if using serverless)

## Troubleshooting

1. **Database not found:**
   - Database is created on first run
   - Ensure write permissions in deployment environment

2. **Port issues:**
   - Use `PORT` environment variable
   - Most platforms set this automatically

3. **Static files not serving:**
   - Ensure `public/` directory is in root
   - Check routing configuration

4. **CORS errors:**
   - `flask-cors` is included
   - Should work automatically

## Recommended Platform

**For easiest deployment:** Use **Railway** or **Render**
- Full Flask support
- Easy GitHub integration
- Free tier available
- Automatic HTTPS

