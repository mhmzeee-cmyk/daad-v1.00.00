/* Developer: محمد محمود الحموز | Dhad Studio */
// ====================================================
// Dhad Studio - Encryption Utility Module
// ====================================================
// Provides AES-256-GCM encryption, hashing, and data protection
// ====================================================

const crypto = require("crypto");

// ── Configuration ────────────────────────────────────────────────────────────
const ALGORITHM = "aes-256-gcm";
const IV_LENGTH = 16;
const SALT_LENGTH = 64;
const TAG_LENGTH = 16;
const KEY_LENGTH = 32;
const PBKDF2_ITERATIONS = 100000;
const SHA256_ROUNDS = 10000;

// ── Key Management ───────────────────────────────────────────────────────────
const MASTER_KEY = process.env.ENCRYPTION_KEY;

if (!MASTER_KEY) {
  console.warn("[Encryption] CRITICAL: No ENCRYPTION_KEY set. Encryption will fail.");
}

let _cachedMasterKey = null;
let _cachedMasterKeyHash = null;

function getMasterKey() {
  if (!MASTER_KEY) {
    throw new Error("ENCRYPTION_KEY or JWT_SECRET must be set for encryption to work");
  }
  const keyHash = crypto.createHash("sha256").update(MASTER_KEY).digest("hex");
  if (_cachedMasterKeyHash === keyHash && _cachedMasterKey) {
    return _cachedMasterKey;
  }
  _cachedMasterKey = deriveKey(MASTER_KEY, "dhad-studio-salt");
  _cachedMasterKeyHash = keyHash;
  return _cachedMasterKey;
}

function deriveKey(secret, salt) {
  return crypto.pbkdf2Sync(secret, salt, PBKDF2_ITERATIONS, KEY_LENGTH, "sha512");
}

// ── AES-256-GCM Encryption ──────────────────────────────────────────────────

/**
 * Encrypt plaintext using AES-256-GCM
 */
function encrypt(plaintext) {
  if (!plaintext) return plaintext;

  const key = getMasterKey();
  const iv = crypto.randomBytes(IV_LENGTH);
  const cipher = crypto.createCipheriv(ALGORITHM, key, iv);

  let encrypted = cipher.update(String(plaintext), "utf8", "hex");
  encrypted += cipher.final("hex");

  const tag = cipher.getAuthTag();

  return `${iv.toString("hex")}:${tag.toString("hex")}:${encrypted}`;
}

/**
 * Decrypt ciphertext using AES-256-GCM
 */
function decrypt(ciphertext) {
  if (!ciphertext) return ciphertext;

  const key = getMasterKey();
  const parts = ciphertext.split(":");
  if (parts.length !== 3) return ciphertext;

  const iv = Buffer.from(parts[0], "hex");
  const tag = Buffer.from(parts[1], "hex");
  const encrypted = parts[2];

  const decipher = crypto.createDecipheriv(ALGORITHM, key, iv);
  decipher.setAuthTag(tag);

  let decrypted = decipher.update(encrypted, "hex", "utf8");
  decrypted += decipher.final("utf8");

  return decrypted;
}

// ── Hashing ──────────────────────────────────────────────────────────────────

/**
 * Create a SHA-256 hash with salt
 */
function hashData(data, salt) {
  if (!data) throw new Error("Data is required for hashing");

  const useSalt = salt || crypto.randomBytes(SALT_LENGTH).toString("hex");
  const hash = crypto
    .pbkdf2Sync(data, useSalt, SHA256_ROUNDS, KEY_LENGTH, "sha512")
    .toString("hex");

  return `${useSalt}:${hash}`;
}

/**
 * Verify a hash
 */
function verifyHash(data, hashWithSalt) {
  if (!data || !hashWithSalt) return false;

  const [salt, hash] = hashWithSalt.split(":");
  if (!salt || !hash) return false;

  const verifyHash = crypto
    .pbkdf2Sync(data, salt, SHA256_ROUNDS, KEY_LENGTH, "sha512")
    .toString("hex");

  return crypto.timingSafeEqual(Buffer.from(hash, "hex"), Buffer.from(verifyHash, "hex"));
}

/**
 * Create an HMAC signature
 */
function createHMAC(data, secret) {
  const key = secret || getMasterKey();
  return crypto.createHmac("sha256", key).update(data).digest("hex");
}

/**
 * Verify an HMAC signature
 */
function verifyHMAC(data, signature, secret) {
  if (!data || !signature) return false;
  const expected = createHMAC(data, secret);
  try {
    return crypto.timingSafeEqual(
      Buffer.from(signature, "hex"),
      Buffer.from(expected, "hex")
    );
  } catch {
    return false;
  }
}

// ── Secure Random Generators ─────────────────────────────────────────────────

function generateToken(length = 32) {
  return crypto.randomBytes(length).toString("hex");
}

function generateApiKey(prefix = "dhad") {
  const key = crypto.randomBytes(32).toString("hex");
  return `${prefix}_${key}`;
}

function generateOTP(length = 6) {
  const min = Math.pow(10, length - 1);
  const max = Math.pow(10, length) - 1;
  return crypto.randomInt(min, max + 1).toString();
}

// ── Data Masking ─────────────────────────────────────────────────────────────

function maskData(data, type = "email") {
  if (!data || typeof data !== "string") return "***";

  switch (type) {
    case "email": {
      const [local, domain] = data.split("@");
      if (!domain) return "***";
      const maskedLocal = local[0] + "***" + local[local.length - 1];
      return `${maskedLocal}@${domain}`;
    }
    case "phone": {
      if (data.length < 4) return "***";
      return "***" + data.slice(-4);
    }
    case "name": {
      if (data.length < 2) return "*";
      return data[0] + "*".repeat(data.length - 2) + data[data.length - 1];
    }
    case "id": {
      if (data.length < 4) return "***";
      return data.slice(0, 2) + "***" + data.slice(-2);
    }
    default:
      return data.slice(0, 2) + "***";
  }
}

// ── Password Utilities ───────────────────────────────────────────────────────

function generatePassword(length = 16) {
  const upper = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
  const lower = "abcdefghijklmnopqrstuvwxyz";
  const numbers = "0123456789";
  const symbols = "!@#$%^&*()_+-=[]{}|;:,.<>?";
  const all = upper + lower + numbers + symbols;

  let password = "";
  password += upper[crypto.randomInt(upper.length)];
  password += lower[crypto.randomInt(lower.length)];
  password += numbers[crypto.randomInt(numbers.length)];
  password += symbols[crypto.randomInt(symbols.length)];

  for (let i = password.length; i < length; i++) {
    password += all[crypto.randomInt(all.length)];
  }

  // Fisher-Yates shuffle for unbiased randomization
  const arr = password.split('');
  for (let i = arr.length - 1; i > 0; i--) {
    const j = crypto.randomInt(i + 1);
    [arr[i], arr[j]] = [arr[j], arr[i]];
  }
  return arr.join('');
}

// ── Exports ──────────────────────────────────────────────────────────────────
module.exports = {
  encrypt,
  decrypt,
  hashData,
  verifyHash,
  createHMAC,
  verifyHMAC,
  generateToken,
  generateApiKey,
  generateOTP,
  generatePassword,
  maskData,
  deriveKey,
  ALGORITHM,
  KEY_LENGTH,
};
