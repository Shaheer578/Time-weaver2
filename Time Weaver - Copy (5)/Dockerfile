# Dockerfile for Time Weaver Backend
# This allows deployment to Render.com, Fly.io, or any Docker-compatible platform

FROM ubuntu:22.04

# Avoid interactive prompts during package installation
ENV DEBIAN_FRONTEND=noninteractive

# Install build dependencies
RUN apt-get update && apt-get install -y \
    g++ \
    sqlite3 \
    libsqlite3-dev \
    make \
    && rm -rf /var/lib/apt/lists/*

# Set working directory
WORKDIR /app

# Copy all source files
COPY server.cpp database.cpp database.h dsa_structures.h sqlite3.c sqlite3.h HolidayData.csv ./

# Compile the application
# Note: On Linux, we don't need -lws2_32 (that's Windows-specific)
RUN g++ -std=c++11 -O2 -o TimeWeaver server.cpp database.cpp sqlite3.c -lpthread -DSQLITE_THREADSAFE=1

# Expose port (Render will set PORT environment variable)
EXPOSE 8080

# Run the server
CMD ["./TimeWeaver"]

