from cryptography.hazmat.primitives.ciphers.aead import AESGCM

key = bytes.fromhex("0000000000000000000000000000000000000000000000000000000000000000")

iv = bytes.fromhex("000000000000000000000000")

data = bytes.fromhex("00810d0a5c0dd53472c6bc644100fe70284dc2b08582d9493bb8c8bbd299e780d7ec") # Key ID + Key  // Key ID = 129 = 0081

#data = bytes.fromhex("007F0d0a5c0dd53472c6bc644100fe70284dc2b08582d9493bb8c8bbd299e780d7ec") # Key ID + Key  // Key ID = 127 = 007F

#data = bytes.fromhex("0081FEDCBA9876543210FEDCBA9876543210FEDCBA9876543210FEDCBA9876543210") # key ID + KEY (old key) 

aesgcm = AESGCM(key)

ct = aesgcm.encrypt(iv, data, None)

ct.hex()

print(ct.hex())

#'ce264d37116dbe5a758879b7fbf363685a2dc17ab224f33dea1a3d35a79fd20e0aa6eaa0791c48e1ef353dba05247b14532e'


#######
def hex_to_vector(hex_string):
    # Remove any leading '0x' if present and convert to uppercase
    hex_string = hex_string.replace("0x", "").upper()
    
    # Ensure the hex string has an even length
    if len(hex_string) % 2 != 0:
        hex_string = '0' + hex_string
    
    # Create a list of integers from the hex string
    vector = [f"0x{hex_string[i:i+2]}" for i in range(0, len(hex_string), 2)]
    
    return vector
    
# Example usage:
hex_input = ct.hex() # Example hexadecimal string
vector_output = hex_to_vector(hex_input)
print(vector_output)    
    
