const { PrismaClient } = require("@prisma/client");
const bcrypt = require("bcryptjs");

const prisma = new PrismaClient();

async function seedAdmin() {
  try {
    await prisma.$connect();
    console.log("[Seed] Connected to Supabase");

    // Find or create a default school for admin
    let school = await prisma.school.findFirst();
    if (!school) {
      school = await prisma.school.create({
        data: {
          name: "Dhad Studio HQ",
          provisionedAt: new Date(),
        },
      });
      console.log(`[Seed] Created school: ${school.name}`);
    }

    // Check if admin already exists
    const existingAdmin = await prisma.user.findFirst({
      where: { role: "ADMIN" },
    });

    if (existingAdmin) {
      console.log(`[Seed] Admin already exists: ${existingAdmin.name} (${existingAdmin.email})`);
      console.log("[Seed] Skipping creation");
      return;
    }

    // Create admin user
    const passwordHash = await bcrypt.hash("admin123", 12);
    const admin = await prisma.user.create({
      data: {
        schoolId: school.id,
        role: "ADMIN",
        name: "System Admin",
        email: "admin@dhadstudio.com",
        nationalId: null,
        passwordHash,
      },
    });

    console.log(`[Seed] Admin created:`);
    console.log(`  Name: ${admin.name}`);
    console.log(`  Email: ${admin.email}`);
    console.log(`  Role: ${admin.role}`);
    console.log(`  ID: ${admin.id}`);
    console.log(`  Password: admin123`);
    console.log(`  School: ${school.name}`);
  } catch (err) {
    console.error("[Seed] Error:", err);
  } finally {
    await prisma.$disconnect();
  }
}

seedAdmin();
