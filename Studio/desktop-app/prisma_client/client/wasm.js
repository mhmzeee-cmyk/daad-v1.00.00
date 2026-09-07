
Object.defineProperty(exports, "__esModule", { value: true });

const {
  Decimal,
  objectEnumValues,
  makeStrictEnum,
  Public,
  getRuntime,
  skip
} = require('@prisma/client/runtime/index-browser.js')


const Prisma = {}

exports.Prisma = Prisma
exports.$Enums = {}

/**
 * Prisma Client JS version: 5.22.0
 * Query Engine version: 605197351a3c8bdd595af2d2a9bc3025bca48ea2
 */
Prisma.prismaVersion = {
  client: "5.22.0",
  engine: "605197351a3c8bdd595af2d2a9bc3025bca48ea2"
}

Prisma.PrismaClientKnownRequestError = () => {
  const runtimeName = getRuntime().prettyName;
  throw new Error(`PrismaClientKnownRequestError is unable to run in this browser environment, or has been bundled for the browser (running in ${runtimeName}).
In case this error is unexpected for you, please report it in https://pris.ly/prisma-prisma-bug-report`,
)};
Prisma.PrismaClientUnknownRequestError = () => {
  const runtimeName = getRuntime().prettyName;
  throw new Error(`PrismaClientUnknownRequestError is unable to run in this browser environment, or has been bundled for the browser (running in ${runtimeName}).
In case this error is unexpected for you, please report it in https://pris.ly/prisma-prisma-bug-report`,
)}
Prisma.PrismaClientRustPanicError = () => {
  const runtimeName = getRuntime().prettyName;
  throw new Error(`PrismaClientRustPanicError is unable to run in this browser environment, or has been bundled for the browser (running in ${runtimeName}).
In case this error is unexpected for you, please report it in https://pris.ly/prisma-prisma-bug-report`,
)}
Prisma.PrismaClientInitializationError = () => {
  const runtimeName = getRuntime().prettyName;
  throw new Error(`PrismaClientInitializationError is unable to run in this browser environment, or has been bundled for the browser (running in ${runtimeName}).
In case this error is unexpected for you, please report it in https://pris.ly/prisma-prisma-bug-report`,
)}
Prisma.PrismaClientValidationError = () => {
  const runtimeName = getRuntime().prettyName;
  throw new Error(`PrismaClientValidationError is unable to run in this browser environment, or has been bundled for the browser (running in ${runtimeName}).
In case this error is unexpected for you, please report it in https://pris.ly/prisma-prisma-bug-report`,
)}
Prisma.NotFoundError = () => {
  const runtimeName = getRuntime().prettyName;
  throw new Error(`NotFoundError is unable to run in this browser environment, or has been bundled for the browser (running in ${runtimeName}).
In case this error is unexpected for you, please report it in https://pris.ly/prisma-prisma-bug-report`,
)}
Prisma.Decimal = Decimal

/**
 * Re-export of sql-template-tag
 */
Prisma.sql = () => {
  const runtimeName = getRuntime().prettyName;
  throw new Error(`sqltag is unable to run in this browser environment, or has been bundled for the browser (running in ${runtimeName}).
In case this error is unexpected for you, please report it in https://pris.ly/prisma-prisma-bug-report`,
)}
Prisma.empty = () => {
  const runtimeName = getRuntime().prettyName;
  throw new Error(`empty is unable to run in this browser environment, or has been bundled for the browser (running in ${runtimeName}).
In case this error is unexpected for you, please report it in https://pris.ly/prisma-prisma-bug-report`,
)}
Prisma.join = () => {
  const runtimeName = getRuntime().prettyName;
  throw new Error(`join is unable to run in this browser environment, or has been bundled for the browser (running in ${runtimeName}).
In case this error is unexpected for you, please report it in https://pris.ly/prisma-prisma-bug-report`,
)}
Prisma.raw = () => {
  const runtimeName = getRuntime().prettyName;
  throw new Error(`raw is unable to run in this browser environment, or has been bundled for the browser (running in ${runtimeName}).
In case this error is unexpected for you, please report it in https://pris.ly/prisma-prisma-bug-report`,
)}
Prisma.validator = Public.validator

/**
* Extensions
*/
Prisma.getExtensionContext = () => {
  const runtimeName = getRuntime().prettyName;
  throw new Error(`Extensions.getExtensionContext is unable to run in this browser environment, or has been bundled for the browser (running in ${runtimeName}).
In case this error is unexpected for you, please report it in https://pris.ly/prisma-prisma-bug-report`,
)}
Prisma.defineExtension = () => {
  const runtimeName = getRuntime().prettyName;
  throw new Error(`Extensions.defineExtension is unable to run in this browser environment, or has been bundled for the browser (running in ${runtimeName}).
In case this error is unexpected for you, please report it in https://pris.ly/prisma-prisma-bug-report`,
)}

/**
 * Shorthand utilities for JSON filtering
 */
Prisma.DbNull = objectEnumValues.instances.DbNull
Prisma.JsonNull = objectEnumValues.instances.JsonNull
Prisma.AnyNull = objectEnumValues.instances.AnyNull

Prisma.NullTypes = {
  DbNull: objectEnumValues.classes.DbNull,
  JsonNull: objectEnumValues.classes.JsonNull,
  AnyNull: objectEnumValues.classes.AnyNull
}



/**
 * Enums
 */

exports.Prisma.TransactionIsolationLevel = makeStrictEnum({
  Serializable: 'Serializable'
});

exports.Prisma.SchoolScalarFieldEnum = {
  id: 'id',
  name: 'name',
  code: 'code',
  email: 'email',
  phone: 'phone',
  address: 'address',
  description: 'description',
  planType: 'planType',
  totalLicenses: 'totalLicenses',
  usedLicenses: 'usedLicenses',
  provisionedAt: 'provisionedAt',
  createdAt: 'createdAt',
  updatedAt: 'updatedAt'
};

exports.Prisma.UserScalarFieldEnum = {
  id: 'id',
  schoolId: 'schoolId',
  classroomId: 'classroomId',
  role: 'role',
  name: 'name',
  email: 'email',
  nationalId: 'nationalId',
  passwordHash: 'passwordHash',
  isActive: 'isActive',
  isVerified: 'isVerified',
  isApproved: 'isApproved',
  isOnboarded: 'isOnboarded',
  tokenVersion: 'tokenVersion',
  resetToken: 'resetToken',
  resetTokenExpiry: 'resetTokenExpiry',
  createdAt: 'createdAt',
  updatedAt: 'updatedAt'
};

exports.Prisma.CourseScalarFieldEnum = {
  id: 'id',
  schoolId: 'schoolId',
  title: 'title',
  description: 'description',
  content: 'content',
  icon: 'icon',
  category: 'category',
  order: 'order',
  unlockTier: 'unlockTier',
  published: 'published',
  createdAt: 'createdAt',
  updatedAt: 'updatedAt'
};

exports.Prisma.StudentScalarFieldEnum = {
  id: 'id',
  schoolId: 'schoolId',
  gradeLevel: 'gradeLevel',
  enrollmentDate: 'enrollmentDate',
  status: 'status',
  gpa: 'gpa',
  createdAt: 'createdAt',
  updatedAt: 'updatedAt'
};

exports.Prisma.LessonScalarFieldEnum = {
  id: 'id',
  schoolId: 'schoolId',
  courseId: 'courseId',
  title: 'title',
  contentSlug: 'contentSlug',
  content: 'content',
  order: 'order',
  duration: 'duration',
  prerequisites: 'prerequisites',
  objectives: 'objectives',
  published: 'published',
  visibility: 'visibility',
  createdAt: 'createdAt',
  updatedAt: 'updatedAt'
};

exports.Prisma.StudentCourseEnrollmentScalarFieldEnum = {
  id: 'id',
  studentId: 'studentId',
  courseId: 'courseId',
  enrolledAt: 'enrolledAt',
  completedAt: 'completedAt'
};

exports.Prisma.ClassroomScalarFieldEnum = {
  id: 'id',
  schoolId: 'schoolId',
  teacherId: 'teacherId',
  name: 'name',
  description: 'description',
  gradeLevel: 'gradeLevel',
  subject: 'subject',
  maxStudents: 'maxStudents',
  isActive: 'isActive',
  inviteCode: 'inviteCode',
  createdAt: 'createdAt',
  updatedAt: 'updatedAt'
};

exports.Prisma.ChallengeScalarFieldEnum = {
  id: 'id',
  authorId: 'authorId',
  title: 'title',
  description: 'description',
  starterCode: 'starterCode',
  expectedOutput: 'expectedOutput',
  dynamicOutput: 'dynamicOutput',
  requirements: 'requirements',
  difficulty: 'difficulty',
  challengeType: 'challengeType',
  published: 'published',
  dailyDate: 'dailyDate',
  tier: 'tier',
  order: 'order',
  estimatedTime: 'estimatedTime',
  points: 'points',
  xpReward: 'xpReward',
  isSystem: 'isSystem',
  popularity: 'popularity',
  createdAt: 'createdAt',
  updatedAt: 'updatedAt',
  schoolId: 'schoolId',
  courseId: 'courseId'
};

exports.Prisma.SubmissionScalarFieldEnum = {
  id: 'id',
  challengeId: 'challengeId',
  studentId: 'studentId',
  code: 'code',
  language: 'language',
  passed: 'passed',
  output: 'output',
  executionTime: 'executionTime',
  memoryUsage: 'memoryUsage',
  syntaxScore: 'syntaxScore',
  performanceScore: 'performanceScore',
  compilationError: 'compilationError',
  timeout: 'timeout',
  lineCount: 'lineCount',
  functionCount: 'functionCount',
  complexity: 'complexity',
  suspicious: 'suspicious',
  suspiciousReason: 'suspiciousReason',
  hmacValid: 'hmacValid',
  aiFeedback: 'aiFeedback',
  createdAt: 'createdAt'
};

exports.Prisma.ProjectScalarFieldEnum = {
  id: 'id',
  studentId: 'studentId',
  lessonId: 'lessonId',
  title: 'title',
  description: 'description',
  code: 'code',
  language: 'language',
  status: 'status',
  score: 'score',
  feedback: 'feedback',
  fileCount: 'fileCount',
  totalSize: 'totalSize',
  createdAt: 'createdAt',
  updatedAt: 'updatedAt'
};

exports.Prisma.AttendanceScalarFieldEnum = {
  id: 'id',
  studentId: 'studentId',
  schoolId: 'schoolId',
  date: 'date',
  status: 'status',
  notes: 'notes',
  recordedBy: 'recordedBy',
  createdAt: 'createdAt'
};

exports.Prisma.ChallengeTierScalarFieldEnum = {
  challengeId: 'challengeId',
  tier: 'tier'
};

exports.Prisma.AssessmentScalarFieldEnum = {
  id: 'id',
  classroomId: 'classroomId',
  title: 'title',
  description: 'description',
  assessmentType: 'assessmentType',
  allowedTime: 'allowedTime',
  startTime: 'startTime',
  endTime: 'endTime',
  totalPoints: 'totalPoints',
  passingScore: 'passingScore',
  isPublished: 'isPublished',
  lockdownMode: 'lockdownMode',
  challengeIds: 'challengeIds',
  questions: 'questions',
  starterCode: 'starterCode',
  expectedOutput: 'expectedOutput',
  instructions: 'instructions',
  createdAt: 'createdAt',
  updatedAt: 'updatedAt'
};

exports.Prisma.StudentAssessmentResultScalarFieldEnum = {
  id: 'id',
  assessmentId: 'assessmentId',
  studentId: 'studentId',
  score: 'score',
  totalPointsEarned: 'totalPointsEarned',
  totalPointsPossible: 'totalPointsPossible',
  challengesAttempted: 'challengesAttempted',
  challengesPassed: 'challengesPassed',
  timeSpent: 'timeSpent',
  startedAt: 'startedAt',
  completedAt: 'completedAt',
  submissionIds: 'submissionIds',
  metadata: 'metadata',
  status: 'status',
  createdAt: 'createdAt',
  updatedAt: 'updatedAt'
};

exports.Prisma.StudentSubmissionScalarFieldEnum = {
  id: 'id',
  submissionId: 'submissionId',
  score: 'score',
  completionTime: 'completionTime',
  tokenCount: 'tokenCount',
  astNodesCount: 'astNodesCount',
  errorCount: 'errorCount',
  warningCount: 'warningCount',
  metadata: 'metadata',
  createdAt: 'createdAt'
};

exports.Prisma.LoginLogScalarFieldEnum = {
  id: 'id',
  userId: 'userId',
  ip: 'ip',
  userAgent: 'userAgent',
  success: 'success',
  createdAt: 'createdAt'
};

exports.Prisma.StudentProfileScalarFieldEnum = {
  id: 'id',
  studentId: 'studentId',
  totalXP: 'totalXP',
  currentLevel: 'currentLevel',
  currentStreak: 'currentStreak',
  longestStreak: 'longestStreak',
  lastActive: 'lastActive',
  lastXPAwarded: 'lastXPAwarded',
  lastActivityDate: 'lastActivityDate',
  createdAt: 'createdAt',
  updatedAt: 'updatedAt'
};

exports.Prisma.DailyXPScalarFieldEnum = {
  id: 'id',
  studentId: 'studentId',
  date: 'date',
  totalXP: 'totalXP',
  submissionsCount: 'submissionsCount'
};

exports.Prisma.AchievementScalarFieldEnum = {
  id: 'id',
  name: 'name',
  description: 'description',
  icon: 'icon',
  criteria: 'criteria',
  points: 'points',
  isSystem: 'isSystem',
  createdAt: 'createdAt'
};

exports.Prisma.UserAchievementScalarFieldEnum = {
  id: 'id',
  userId: 'userId',
  achievementId: 'achievementId',
  awardedAt: 'awardedAt',
  metadata: 'metadata',
  studentProfileId: 'studentProfileId'
};

exports.Prisma.ActivityLogScalarFieldEnum = {
  id: 'id',
  userId: 'userId',
  action: 'action',
  details: 'details',
  createdAt: 'createdAt'
};

exports.Prisma.CloudWorkspaceScalarFieldEnum = {
  id: 'id',
  studentId: 'studentId',
  challengeId: 'challengeId',
  savedCode: 'savedCode',
  language: 'language',
  lastSavedAt: 'lastSavedAt',
  createdAt: 'createdAt',
  updatedAt: 'updatedAt'
};

exports.Prisma.SecurityAlertScalarFieldEnum = {
  id: 'id',
  studentId: 'studentId',
  schoolId: 'schoolId',
  alertType: 'alertType',
  severity: 'severity',
  details: 'details',
  resolved: 'resolved',
  resolvedBy: 'resolvedBy',
  resolvedAt: 'resolvedAt',
  createdAt: 'createdAt'
};

exports.Prisma.InvitationScalarFieldEnum = {
  id: 'id',
  studentEmail: 'studentEmail',
  studentName: 'studentName',
  classroomId: 'classroomId',
  schoolId: 'schoolId',
  schoolName: 'schoolName',
  token: 'token',
  tokenExpiry: 'tokenExpiry',
  isActivated: 'isActivated',
  userId: 'userId',
  activatedAt: 'activatedAt',
  createdAt: 'createdAt',
  updatedAt: 'updatedAt'
};

exports.Prisma.ChallengeVerificationScalarFieldEnum = {
  id: 'id',
  studentId: 'studentId',
  taskId: 'taskId',
  timestamp: 'timestamp',
  signature: 'signature',
  ip: 'ip',
  userAgent: 'userAgent',
  createdAt: 'createdAt'
};

exports.Prisma.SortOrder = {
  asc: 'asc',
  desc: 'desc'
};

exports.Prisma.NullsOrder = {
  first: 'first',
  last: 'last'
};


exports.Prisma.ModelName = {
  School: 'School',
  User: 'User',
  Course: 'Course',
  Student: 'Student',
  Lesson: 'Lesson',
  StudentCourseEnrollment: 'StudentCourseEnrollment',
  Classroom: 'Classroom',
  Challenge: 'Challenge',
  Submission: 'Submission',
  Project: 'Project',
  Attendance: 'Attendance',
  ChallengeTier: 'ChallengeTier',
  Assessment: 'Assessment',
  StudentAssessmentResult: 'StudentAssessmentResult',
  StudentSubmission: 'StudentSubmission',
  LoginLog: 'LoginLog',
  StudentProfile: 'StudentProfile',
  DailyXP: 'DailyXP',
  Achievement: 'Achievement',
  UserAchievement: 'UserAchievement',
  ActivityLog: 'ActivityLog',
  CloudWorkspace: 'CloudWorkspace',
  SecurityAlert: 'SecurityAlert',
  Invitation: 'Invitation',
  ChallengeVerification: 'ChallengeVerification'
};

/**
 * This is a stub Prisma Client that will error at runtime if called.
 */
class PrismaClient {
  constructor() {
    return new Proxy(this, {
      get(target, prop) {
        let message
        const runtime = getRuntime()
        if (runtime.isEdge) {
          message = `PrismaClient is not configured to run in ${runtime.prettyName}. In order to run Prisma Client on edge runtime, either:
- Use Prisma Accelerate: https://pris.ly/d/accelerate
- Use Driver Adapters: https://pris.ly/d/driver-adapters
`;
        } else {
          message = 'PrismaClient is unable to run in this browser environment, or has been bundled for the browser (running in `' + runtime.prettyName + '`).'
        }
        
        message += `
If this is unexpected, please open an issue: https://pris.ly/prisma-prisma-bug-report`

        throw new Error(message)
      }
    })
  }
}

exports.PrismaClient = PrismaClient

Object.assign(exports, Prisma)
