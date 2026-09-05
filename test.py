import requests

# Define the local server URL
url = "http://localhost:8085/data.json"

try:
    # Send the GET request
    response = requests.get(url)
    
    # Raise an exception for HTTP error codes (e.g., 404, 500)
    response.raise_for_status()
    
    # Parse the response directly into a Python dict/list
    data = response.json()
    print("Successfully fetched JSON data!")
    print(data)

    print(data.Children[0])

except requests.exceptions.ConnectionError:
    print("Could not connect to the server. Is it running on port 8050?")
except requests.exceptions.HTTPError as http_err:
    print(f"HTTP error occurred: {http_err}")
except requests.exceptions.JSONDecodeError:
    print("The server responded, but the data was not valid JSON.")
