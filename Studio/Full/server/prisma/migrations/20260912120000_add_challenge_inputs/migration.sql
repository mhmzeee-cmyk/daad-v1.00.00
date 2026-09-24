-- Add inputs JSON array to Challenge (per-challenge stdin queue for ادخل).
-- Values are JSON-encoded by the seeder; the server coerces numeric strings
-- exactly like the browser prompt() path. Exhausted queue reads as 0 (legacy).
ALTER TABLE "Challenge" ADD COLUMN "inputs" TEXT NOT NULL DEFAULT '[]';
