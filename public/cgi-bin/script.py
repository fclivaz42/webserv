#!/usr/bin/env python3

import os
import sys
import urllib.parse

def main():
    # Read environment variables
    # request_method = os.getenv('REQUEST_METHOD')
    # content_type = os.getenv('CONTENT_TYPE')
    # # content_length = os.getenv('CONTENT_LENGTH', '0')

    # Read the request body
    #input_data = sys.stdin.read(int(content_length))

    # Parse query parameters if content type is 'application/x-www-form-urlencoded'
    # #if content_type == 'application/x-www-form-urlencoded':
    #     form_data = urllib.parse.parse_qs(input_data)
        
    #     # Extract individual form fields
    #     name = form_data.get('name', [''])[0]
    #     email = form_data.get('email', [''])[0]
    #     message = form_data.get('message', [''])[0]
#     else:
    name, email, message = '', '', ''

    # Prepare the HTTP response
    print("Content-Type: text/html")
    print()  # End of headers
    print("<html>")
    print("<head><title>CGI Response</title></head>")
    print("<body>")
    print("<h1>Form Submission Results</h1>")
    print(f"<p>Name: {name}</p>")
    print(f"<p>Email: {email}</p>")
    print(f"<p>Message: {message}</p>")
    print("</body>")
    print("</html>")

if __name__ == "__main__":
    main()
