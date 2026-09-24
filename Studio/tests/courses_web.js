const challenges = [
  {
    title: "\u0623\u0633\u0627\u0633\u064a\u0627\u062a \u0627\u0644\u0639\u0646\u0627\u0635\u0631: \u0627\u0644\u0645\u062b\u0627\u0644",
    description: "\u062a\u0639\u0644\u0645 \u0643\u064a\u0641\u064a\u0629 \u0637\u0628\u0627\u0639 \u0627\u0644\u0639\u0646\u0627\u0635\u0631 \u0627\u0644\u0623\u0633\u0627\u0633\u064a\u0629 \u0641\u064a Dhad \u0628\u0627\u0633\u062a\u062e\u062f\u0627\u0645 \u0637\u0628\u0627\u0639\u0629 \u0648\u0646\u0635",
    starterCode: "\u0637\u0628\u0627\u0639\u0629(\u0646\u0635(\"\u0645\u0631\u062d\u0628\u0627\u064b \u0628\u0627\u0644\u0639\u0627\u0644\u0645\"));\n\u0637\u0628\u0627\u0639\u0629(\u0646\u0635(\"\u0623\u0648\u0644 \u0635\u0641\u062d\u0629 \u0648\u064a\u0628\"));",
    expectedOutput: "\u0645\u0631\u062d\u0628\u0627\u064b \u0628\u0627\u0644\u0639\u0627\u0644\u0645\n\u0623\u0648\u0644 \u0635\u0641\u062d\u0629 \u0648\u064a\u0628",
    requirements: [
      { type: "must_use_keyword", keyword: "\u0637\u0628\u0627\u0639\u0629" },
      { type: "must_use_keyword", keyword: "\u0646\u0635" }
    ],
    tier: 1,
    order: 1,
    xpReward: 20,
    points: 20,
    difficulty: "BEGINNER",
    dynamicOutput: "",
    courseId: "WEBDEV"
  },
  {
    title: "\u0623\u0633\u0627\u0633\u064a\u0627\u062a \u0627\u0644\u0639\u0646\u0627\u0635\u0631: \u062a\u0645\u0631\u064a\u0646 1",
    description: "\u0627\u0643\u062a\u0628 \u0643\u0648\u062f Dhad \u0644\u0637\u0628\u0627\u0639 \u0646\u0635 \u0639\u0644\u0649 \u0627\u0644\u0634\u0627\u0634\u0629",
    starterCode: "",
    expectedOutput: "\u0646\u0635 \u0639\u0644\u0649 \u0627\u0644\u0634\u0627\u0634\u0629",
    requirements: [
      { type: "must_use_keyword", keyword: "\u0637\u0628\u0627\u0639\u0629" }
    ],
    tier: 1,
    order: 2,
    xpReward: 20,
    points: 20,
    difficulty: "BEGINNER",
    dynamicOutput: "",
    courseId: "WEBDEV"
  },
  {
    title: "\u0623\u0633\u0627\u0633\u064a\u0627\u062a \u0627\u0644\u0639\u0646\u0627\u0635\u0631: \u062a\u0645\u0631\u064a\u0646 2",
    description: "\u0623\u0646\u0634\u0626 \u0635\u0641\u062d\u0629 \u0648\u064a\u0628 \u0628\u0633\u064a\u0637\u0629 \u062a\u0637\u0628\u0639 \u0623\u0643\u062b\u0631 \u0645\u0646 \u0646\u0635",
    starterCode: "",
    expectedOutput: "\u0635\u0641\u062d\u0629 \u0648\u064a\u0628 \u0628\u0633\u064a\u0637\u0629",
    requirements: [
      { type: "must_use_keyword", keyword: "\u0637\u0628\u0627\u0639\u0629" },
      { type: "must_use_keyword", keyword: "\u0646\u0635" }
    ],
    tier: 1,
    order: 3,
    xpReward: 20,
    points: 20,
    difficulty: "BEGINNER",
    dynamicOutput: "",
    courseId: "WEBDEV"
  },
  {
    title: "\u0627\u0644\u0639\u0646\u0627\u0635\u0631 \u0648\u0627\u0644\u0633\u0645\u0627\u062a: \u0627\u0644\u0645\u062b\u0627\u0644",
    description: "\u062a\u0639\u0644\u0645 \u0643\u064a\u0641\u064a\u0629 \u0627\u0633\u062a\u062e\u062f\u0627\u0645 \u0627\u0644\u0633\u0645\u0627\u062a \u0645\u0639 \u0627\u0644\u0639\u0646\u0627\u0635\u0631 \u0641\u064a Dhad",
    starterCode: "\u0639\u0646\u0627\u0635\u0631(\"p\", \"\u0646\u0635 \u0623\u0648\u0644\", \u0633\u0645\u0629(\"class\", \"text\"));\n\u0639\u0646\u0627\u0635\u0631(\"a\", \"\u0631\u0627\u0628\u0637\", \u0633\u0645\u0629(\"href\", \"https://example.com\"));",
    expectedOutput: "<p class=\"text\">\u0646\u0635 \u0623\u0648\u0644</p>\n<a href=\"https://example.com\">\u0631\u0627\u0628\u0637</a>",
    requirements: [
      { type: "must_use_keyword", keyword: "\u0639\u0646\u0627\u0635\u0631" }
    ],
    tier: 2,
    order: 1,
    xpReward: 20,
    points: 20,
    difficulty: "BEGINNER",
    dynamicOutput: "",
    courseId: "WEBDEV"
  },
  {
    title: "\u0627\u0644\u0639\u0646\u0627\u0635\u0631 \u0648\u0627\u0644\u0633\u0645\u0627\u062a: \u062a\u0645\u0631\u064a\u0646 1",
    description: "\u0627\u0643\u062a\u0628 \u0643\u0648\u062f \u0644\u0637\u0628\u0627\u0639 \u0633\u0645\u0627\u062a \u0627\u0644\u0639\u0646\u0627\u0635\u0631",
    starterCode: "",
    expectedOutput: "\u0633\u0645\u0627\u062a \u0627\u0644\u0639\u0646\u0627\u0635\u0631",
    requirements: [
      { type: "must_use_keyword", keyword: "\u0639\u0646\u0627\u0635\u0631" }
    ],
    tier: 2,
    order: 2,
    xpReward: 20,
    points: 20,
    difficulty: "BEGINNER",
    dynamicOutput: "",
    courseId: "WEBDEV"
  },
  {
    title: "\u0627\u0644\u0639\u0646\u0627\u0635\u0631 \u0648\u0627\u0644\u0633\u0645\u0627\u062a: \u062a\u0645\u0631\u064a\u0646 2",
    description: "\u0623\u0646\u0634\u0626 \u0639\u0646\u0627\u0635\u0631 \u0628\u0639\u062f\u0629 \u0633\u0645\u0627\u062a \u0645\u062e\u062a\u0644\u0641\u0629",
    starterCode: "",
    expectedOutput: "\u0639\u0646\u0627\u0635\u0631 \u0628\u0633\u0645\u0627\u062a \u0645\u062e\u062a\u0644\u0641\u0629",
    requirements: [
      { type: "must_use_keyword", keyword: "\u0639\u0646\u0627\u0635\u0631" },
      { type: "must_use_keyword", keyword: "\u0646\u0635" }
    ],
    tier: 2,
    order: 3,
    xpReward: 20,
    points: 20,
    difficulty: "BEGINNER",
    dynamicOutput: "",
    courseId: "WEBDEV"
  },
  {
    title: "\u0627\u0644\u0646\u0635\u0648\u0635 \u0648\u0627\u0644\u0639\u0646\u0627\u0648\u064a\u0646: \u0627\u0644\u0645\u062b\u0627\u0644",
    description: "\u062a\u0639\u0644\u0645 \u0643\u064a\u0641\u064a\u0629 \u0625\u0646\u0634\u0627\u0621 \u0627\u0644\u0639\u0646\u0627\u0648\u064a\u0646 \u0648\u0627\u0644\u0641\u0642\u0631\u0627\u062a \u0641\u064a Dhad",
    starterCode: "\u0639\u0646\u0627\u0635\u0631(\"h1\", \"\u0627\u0644\u0639\u0646\u0648\u0627\u0646 \u0627\u0644\u0631\u0626\u064a\u0633\u064a\");\n\u0639\u0646\u0627\u0635\u0631(\"h2\", \"\u0639\u0646\u0648\u0627\u0646 \u0641\u0631\u0639\u064a\");\n\u0639\u0646\u0627\u0635\u0631(\"p\", \"\u0647\u0630\u0647 \u0641\u0642\u0631\u0629 \u0646\u0635\u064a\u0629 \u0623\u0648\u0644\u0649\");\n\u0639\u0646\u0627\u0635\u0631(\"p\", \"\u0647\u0630\u0647 \u0641\u0642\u0631\u0629 \u0646\u0635\u064a\u0629 \u062b\u0627\u0646\u064a\u0629\");",
    expectedOutput: "<h1>\u0627\u0644\u0639\u0646\u0648\u0627\u0646 \u0627\u0644\u0631\u0626\u064a\u0633\u064a</h1>\n<h2>\u0639\u0646\u0648\u0627\u0646 \u0641\u0631\u0639\u064a</h2>\n<p>\u0647\u0630\u0647 \u0641\u0642\u0631\u0629 \u0646\u0635\u064a\u0629 \u0623\u0648\u0644\u0649</p>\n<p>\u0647\u0630\u0647 \u0641\u0642\u0631\u0629 \u0646\u0635\u064a\u0629 \u062b\u0627\u0646\u064a\u0629</p>",
    requirements: [
      { type: "must_use_keyword", keyword: "\u0639\u0646\u0627\u0635\u0631" },
      { type: "must_use_keyword", keyword: "\u0646\u0635" }
    ],
    tier: 3,
    order: 1,
    xpReward: 20,
    points: 20,
    difficulty: "BEGINNER",
    dynamicOutput: "",
    courseId: "WEBDEV"
  },
  {
    title: "\u0627\u0644\u0646\u0635\u0648\u0635 \u0648\u0627\u0644\u0639\u0646\u0627\u0648\u064a\u0646: \u062a\u0645\u0631\u064a\u0646 1",
    description: "\u0627\u0646\u0634\u0626 \u0635\u0641\u062d\u0629 \u062a\u062d\u062a\u0648\u064a \u0639\u0644\u0649 \u0639\u0646\u0627\u0648\u064a\u0646 \u0648\u0641\u0642\u0631\u0627\u062a \u0646\u0635\u064a\u0629",
    starterCode: "",
    expectedOutput: "\u0635\u0641\u062d\u0629 \u0628\u0639\u0646\u0627\u0648\u064a\u0646 \u0648\u0641\u0642\u0631\u0627\u062a",
    requirements: [
      { type: "must_use_keyword", keyword: "\u0639\u0646\u0627\u0635\u0631" }
    ],
    tier: 3,
    order: 2,
    xpReward: 20,
    points: 20,
    difficulty: "BEGINNER",
    dynamicOutput: "",
    courseId: "WEBDEV"
  },
  {
    title: "\u0627\u0644\u0646\u0635\u0648\u0635 \u0648\u0627\u0644\u0639\u0646\u0627\u0648\u064a\u0646: \u062a\u0645\u0631\u064a\u0646 2",
    description: "\u0635\u0645\u0645 \u0635\u0641\u062d\u0629 \u062a\u062d\u062a\u0648\u064a \u0639\u0644\u0649 \u0646\u0635\u0648\u0635 \u0648\u0639\u0646\u0627\u0648\u064a\u0646 \u0645\u062a\u0639\u062f\u062f\u0629",
    starterCode: "",
    expectedOutput: "\u0635\u0641\u062d\u0629 \u0628\u0646\u0635\u0648\u0635 \u0645\u062a\u0639\u062f\u062f\u0629",
    requirements: [
      { type: "must_use_keyword", keyword: "\u0639\u0646\u0627\u0635\u0631" },
      { type: "must_use_keyword", keyword: "\u0646\u0635" }
    ],
    tier: 3,
    order: 3,
    xpReward: 20,
    points: 20,
    difficulty: "BEGINNER",
    dynamicOutput: "",
    courseId: "WEBDEV"
  },
  {
    title: "\u0627\u0644\u0631\u0648\u0627\u0628\u0637 \u0648\u0627\u0644\u0635\u0648\u0631: \u0627\u0644\u0645\u062b\u0627\u0644",
    description: "\u062a\u0639\u0644\u0645 \u0643\u064a\u0641\u064a\u0629 \u0625\u0646\u0634\u0627\u0621 \u0627\u0644\u0631\u0648\u0627\u0628\u0637 \u0648\u0625\u062f\u0631\u0627\u062c \u0627\u0644\u0635\u0648\u0631 \u0641\u064a Dhad",
    starterCode: "\u0639\u0646\u0627\u0635\u0631(\"a\", \"\u0632\u064a\u0627\u0631\u0629 \u0627\u0644\u0645\u0648\u0642\u0639\", \u0633\u0645\u0629(\"href\", \"https://example.com\"));\n\u0639\u0646\u0627\u0635\u0631(\"img\", \"\", \u0633\u0645\u0629(\"src\", \"image.jpg\"), \u0633\u0645\u0629(\"alt\", \"\u0635\u0648\u0631\u0629 \u062a\u0648\u0636\u064a\u062d\u064a\u0629\"));",
    expectedOutput: "<a href=\"https://example.com\">\u0632\u064a\u0627\u0631\u0629 \u0627\u0644\u0645\u0648\u0642\u0639</a>\n<img src=\"image.jpg\" alt=\"\u0635\u0648\u0631\u0629 \u062a\u0648\u0636\u064a\u062d\u064a\u0629\">",
    requirements: [
      { type: "must_use_keyword", keyword: "\u0639\u0646\u0627\u0635\u0631" }
    ],
    tier: 4,
    order: 1,
    xpReward: 25,
    points: 25,
    difficulty: "INTERMEDIATE",
    dynamicOutput: "",
    courseId: "WEBDEV"
  },
  {
    title: "\u0627\u0644\u0631\u0648\u0627\u0628\u0637 \u0648\u0627\u0644\u0635\u0648\u0631: \u062a\u0645\u0631\u064a\u0646 1",
    description: "\u0627\u0646\u0634\u0626 \u0635\u0641\u062d\u0629 \u062a\u062d\u062a\u0648\u064a \u0639\u0644\u0649 \u0631\u0627\u0628\u0637 \u0648\u0635\u0648\u0631\u0629",
    starterCode: "",
    expectedOutput: "\u0635\u0641\u062d\u0629 \u0628\u0631\u0627\u0628\u0637 \u0648\u0635\u0648\u0631\u0629",
    requirements: [
      { type: "must_use_keyword", keyword: "\u0639\u0646\u0627\u0635\u0631" }
    ],
    tier: 4,
    order: 2,
    xpReward: 25,
    points: 25,
    difficulty: "INTERMEDIATE",
    dynamicOutput: "",
    courseId: "WEBDEV"
  },
  {
    title: "\u0627\u0644\u0631\u0648\u0627\u0628\u0637 \u0648\u0627\u0644\u0635\u0648\u0631: \u062a\u0645\u0631\u064a\u0646 2",
    description: "\u0635\u0645\u0645 \u0645\u0639\u0631\u0636 \u0635\u0648\u0631 \u064a\u062d\u062a\u0648\u064a \u0639\u0644\u0649 \u0635\u0648\u0631 \u0648\u0631\u0648\u0627\u0628\u0637 \u0645\u062a\u0639\u062f\u062f\u0629",
    starterCode: "",
    expectedOutput: "\u0645\u0639\u0631\u0636 \u0635\u0648\u0631",
    requirements: [
      { type: "must_use_keyword", keyword: "\u0639\u0646\u0627\u0635\u0631" },
      { type: "must_use_keyword", keyword: "\u062c\u062f\u064a\u062f" }
    ],
    tier: 4,
    order: 3,
    xpReward: 25,
    points: 25,
    difficulty: "INTERMEDIATE",
    dynamicOutput: "",
    courseId: "WEBDEV"
  },
  {
    title: "\u0627\u0644\u0642\u0648\u0627\u0626\u0645 \u0648\u0627\u0644\u062c\u062f\u0627\u0648\u064a\u0644: \u0627\u0644\u0645\u062b\u0627\u0644",
    description: "\u062a\u0639\u0644\u0645 \u0643\u064a\u0641\u064a\u0629 \u0625\u0646\u0634\u0627\u0621 \u0627\u0644\u0642\u0648\u0627\u0626\u0645 \u0627\u0644\u0645\u0646\u0633\u0627\u0637\u0642\u0629 \u0648\u063a\u064a\u0631 \u0627\u0644\u0645\u0646\u0633\u0627\u0637\u0642\u0629 \u0648\u0627\u0644\u062c\u062f\u0627\u0648\u064a\u0644",
    starterCode: "\u0639\u0646\u0627\u0635\u0631(\"ul\", [\u0639\u0646\u0627\u0635\u0631(\"li\", \"\u0627\u0644\u0628\u0646\u062f \u0627\u0644\u0623\u0648\u0644\"), \u0639\u0646\u0627\u0635\u0631(\"li\", \"\u0627\u0644\u0628\u0646\u062f \u0627\u0644\u062b\u0627\u0646\u064a\")]);\n\u0639\u0646\u0627\u0635\u0631(\"ol\", [\u0639\u0646\u0627\u0635\u0631(\"li\", \"\u0627\u0644\u062e\u0637\u0648\u0629 \u0627\u0644\u0623\u0648\u0644\u0649\"), \u0639\u0646\u0627\u0635\u0631(\"li\", \"\u0627\u0644\u062e\u0637\u0648\u0629 \u0627\u0644\u062b\u0627\u0646\u064a\u0629\")]);\n\u0639\u0646\u0627\u0635\u0631(\"table\", [\n  \u0639\u0646\u0627\u0635\u0631(\"tr\", [\u0639\u0646\u0627\u0635\u0631(\"th\", \"\u0627\u0644\u0627\u0633\u0645\"), \u0639\u0646\u0627\u0635\u0631(\"th\", \"\u0627\u0644\u0639\u0645\u0631\")]),\n  \u0639\u0646\u0627\u0635\u0631(\"tr\", [\u0639\u0646\u0627\u0635\u0631(\"td\", \"\u0623\u062d\u0645\u062f\"), \u0639\u0646\u0627\u0635\u0631(\"td\", \"25\")])\n]);",
    expectedOutput: "<ul><li>\u0627\u0644\u0628\u0646\u062f \u0627\u0644\u0623\u0648\u0644</li><li>\u0627\u0644\u0628\u0646\u062f \u0627\u0644\u062b\u0627\u0646\u064a</li></ul>\n<ol><li>\u0627\u0644\u062e\u0637\u0648\u0629 \u0627\u0644\u0623\u0648\u0644\u0649</li><li>\u0627\u0644\u062e\u0637\u0648\u0629 \u0627\u0644\u062b\u0627\u0646\u064a\u0629</li></ol>\n<table><tr><th>\u0627\u0644\u0627\u0633\u0645</th><th>\u0627\u0644\u0639\u0645\u0631</th></tr><tr><td>\u0623\u062d\u0645\u062f</td><td>25</td></tr></table>",
    requirements: [
      { type: "must_use_keyword", keyword: "\u0639\u0646\u0627\u0635\u0631" }
    ],
    tier: 5,
    order: 1,
    xpReward: 25,
    points: 25,
    difficulty: "INTERMEDIATE",
    dynamicOutput: "",
    courseId: "WEBDEV"
  },
  {
    title: "\u0627\u0644\u0642\u0648\u0627\u0626\u0645 \u0648\u0627\u0644\u062c\u062f\u0627\u0648\u064a\u0644: \u062a\u0645\u0631\u064a\u0646 1",
    description: "\u0627\u0646\u0634\u0626 \u0642\u0627\u0626\u0645\u0629 \u0645\u0646\u0633\u0627\u0637\u0642\u0629 \u0648\u062c\u062f\u0648\u0644 \u0628\u064a\u0627\u0646\u0627\u062a \u0628\u0633\u064a\u0637",
    starterCode: "",
    expectedOutput: "\u0642\u0627\u0626\u0645\u0629 \u0648\u062c\u062f\u0648\u0644",
    requirements: [
      { type: "must_use_keyword", keyword: "\u0639\u0646\u0627\u0635\u0631" }
    ],
    tier: 5,
    order: 2,
    xpReward: 25,
    points: 25,
    difficulty: "INTERMEDIATE",
    dynamicOutput: "",
    courseId: "WEBDEV"
  },
  {
    title: "\u0627\u0644\u0642\u0648\u0627\u0626\u0645 \u0648\u0627\u0644\u062c\u062f\u0627\u0648\u064a\u0644: \u062a\u0645\u0631\u064a\u0646 2",
    description: "\u0635\u0645\u0645 \u0635\u0641\u062d\u0629 \u062a\u062d\u062a\u0648\u064a \u0639\u0644\u0649 \u0642\u0648\u0627\u0626\u0645 \u0645\u062a\u0639\u062f\u062f\u0629 \u0648\u062c\u062f\u0627\u0648\u064a\u0644 \u0628\u064a\u0627\u0646\u0627\u062a",
    starterCode: "",
    expectedOutput: "\u0635\u0641\u062d\u0629 \u0628\u0642\u0648\u0627\u0626\u0645 \u0648\u062c\u062f\u0627\u0648\u064a\u0644",
    requirements: [
      { type: "must_use_keyword", keyword: "\u0639\u0646\u0627\u0635\u0631" },
      { type: "must_use_keyword", keyword: "\u062c\u062f\u064a\u062f" }
    ],
    tier: 5,
    order: 3,
    xpReward: 25,
    points: 25,
    difficulty: "INTERMEDIATE",
    dynamicOutput: "",
    courseId: "WEBDEV"
  },
  {
    title: "\u0627\u0644\u062a\u0646\u0633\u064a\u0642 \u0627\u0644\u0623\u0633\u0627\u0633\u064a: \u0627\u0644\u0645\u062b\u0627\u0644",
    description: "\u062a\u0639\u0644\u0645 \u0643\u064a\u0641\u064a\u0629 \u062a\u0637\u0628\u064a\u0642 \u0627\u0644\u062a\u0646\u0633\u064a\u0642 \u0627\u0644\u0623\u0633\u0627\u0633\u064a \u0639\u0644\u0649 \u0627\u0644\u0639\u0646\u0627\u0635\u0631 \u0628\u0627\u0633\u062a\u062e\u062f\u0627\u0645 CSS \u0641\u064a Dhad",
    starterCode: "\u062a\u0646\u0633\u064a\u0642(\"p\", \"color\", \"blue\");\n\u062a\u0646\u0633\u064a\u0642(\"h1\", \"font-size\", \"24px\");\n\u062a\u0646\u0633\u064a\u0642(\"body\", \"margin\", \"0\");",
    expectedOutput: "p { color: blue; }\nh1 { font-size: 24px; }\nbody { margin: 0; }",
    requirements: [
      { type: "must_use_keyword", keyword: "\u062a\u0646\u0633\u064a\u0642" }
    ],
    tier: 6,
    order: 1,
    xpReward: 25,
    points: 25,
    difficulty: "INTERMEDIATE",
    dynamicOutput: "",
    courseId: "WEBDEV"
  },
  {
    title: "\u0627\u0644\u062a\u0646\u0633\u064a\u0642 \u0627\u0644\u0623\u0633\u0627\u0633\u064a: \u062a\u0645\u0631\u064a\u0646 1",
    description: "\u0637\u0628\u0651\u0642 \u062a\u0646\u0633\u064a\u0642\u0627\u062a CSS \u0639\u0644\u0649 \u0639\u0646\u0627\u0635\u0631 \u0627\u0644\u0635\u0641\u062d\u0629",
    starterCode: "",
    expectedOutput: "\u062a\u0646\u0633\u064a\u0642\u0627\u062a CSS",
    requirements: [
      { type: "must_use_keyword", keyword: "\u062a\u0646\u0633\u064a\u0642" }
    ],
    tier: 6,
    order: 2,
    xpReward: 25,
    points: 25,
    difficulty: "INTERMEDIATE",
    dynamicOutput: "",
    courseId: "WEBDEV"
  },
  {
    title: "\u0627\u0644\u062a\u0646\u0633\u064a\u0642 \u0627\u0644\u0623\u0633\u0627\u0633\u064a: \u062a\u0645\u0631\u064a\u0646 2",
    description: "\u0635\u0645\u0645 \u0635\u0641\u062d\u0629 \u0628\u0623\u0644\u0648\u0627\u0646 \u0648\u062a\u0646\u0633\u064a\u0642\u0627\u062a \u0645\u062a\u0639\u062f\u062f\u0629",
    starterCode: "",
    expectedOutput: "\u0635\u0641\u062d\u0629 \u0645\u062a\u0646\u0633\u0642\u0629",
    requirements: [
      { type: "must_use_keyword", keyword: "\u062a\u0646\u0633\u064a\u0642" },
      { type: "must_use_keyword", keyword: "\u0639\u0646\u0627\u0635\u0631" }
    ],
    tier: 6,
    order: 3,
    xpReward: 25,
    points: 25,
    difficulty: "INTERMEDIATE",
    dynamicOutput: "",
    courseId: "WEBDEV"
  },
  {
    title: "\u0627\u0644\u0623\u0644\u0648\u0627\u0646 \u0648\u0627\u0644\u062e\u0637\u0648\u0637: \u0627\u0644\u0645\u062b\u0627\u0644",
    description: "\u062a\u0639\u0644\u0645 \u0643\u064a\u0641\u064a\u0629 \u062a\u063a\u064a\u064a\u0631 \u0627\u0644\u0623\u0644\u0648\u0627\u0646 \u0648\u0627\u0644\u062e\u0637\u0648\u0637 \u0644\u0644\u0639\u0646\u0627\u0635\u0631 \u0641\u064a Dhad",
    starterCode: "\u062a\u0646\u0633\u064a\u0642(\"p\", \"color\", \"#333333\");\n\u062a\u0646\u0633\u064a\u0642(\"p\", \"font-family\", \"Arial, sans-serif\");\n\u062a\u0646\u0633\u064a\u0642(\"h1\", \"color\", \"red\");\n\u062a\u0646\u0633\u064a\u0642(\"h1\", \"font-size\", \"32px\");",
    expectedOutput: "p { color: #333333; font-family: Arial, sans-serif; }\nh1 { color: red; font-size: 32px; }",
    requirements: [
      { type: "must_use_keyword", keyword: "\u062a\u0646\u0633\u064a\u0642" }
    ],
    tier: 7,
    order: 1,
    xpReward: 25,
    points: 25,
    difficulty: "INTERMEDIATE",
    dynamicOutput: "",
    courseId: "WEBDEV"
  },
  {
    title: "\u0627\u0644\u0623\u0644\u0648\u0627\u0646 \u0648\u0627\u0644\u062e\u0637\u0648\u0637: \u062a\u0645\u0631\u064a\u0646 1",
    description: "\u063a\u064a\u0651\u0631 \u0623\u0644\u0648\u0627\u0646 \u0648\u062e\u0637\u0648\u0637 \u0639\u0646\u0627\u0635\u0631 \u0627\u0644\u0635\u0641\u062d\u0629",
    starterCode: "",
    expectedOutput: "\u0623\u0644\u0648\u0627\u0646 \u0648\u062e\u0637\u0648\u0637 \u0645\u062e\u062a\u0644\u0641\u0629",
    requirements: [
      { type: "must_use_keyword", keyword: "\u062a\u0646\u0633\u064a\u0642" }
    ],
    tier: 7,
    order: 2,
    xpReward: 25,
    points: 25,
    difficulty: "INTERMEDIATE",
    dynamicOutput: "",
    courseId: "WEBDEV"
  },
  {
    title: "\u0627\u0644\u0623\u0644\u0648\u0627\u0646 \u0648\u0627\u0644\u062e\u0637\u0648\u0637: \u062a\u0645\u0631\u064a\u0646 2",
    description: "\u0635\u0645\u0645 \u0635\u0641\u062d\u0629 \u0628\u0623\u0644\u0648\u0627\u0646 \u0648\u062e\u0637\u0648\u0637 \u0645\u062a\u0646\u0627\u0633\u0642\u0629",
    starterCode: "",
    expectedOutput: "\u0635\u0641\u062d\u0629 \u0628\u0623\u0644\u0648\u0627\u0646 \u0645\u062a\u0646\u0627\u0633\u0642\u0629",
    requirements: [
      { type: "must_use_keyword", keyword: "\u062a\u0646\u0633\u064a\u0642" },
      { type: "must_use_keyword", keyword: "\u0639\u0646\u0627\u0635\u0631" }
    ],
    tier: 7,
    order: 3,
    xpReward: 25,
    points: 25,
    difficulty: "INTERMEDIATE",
    dynamicOutput: "",
    courseId: "WEBDEV"
  },
  {
    title: "\u0627\u0644\u062a\u062e\u0637\u064a\u0637 \u0648\u0627\u0644\u0645\u0631\u0648\u0646\u0629: \u0627\u0644\u0645\u062b\u0627\u0644",
    description: "\u062a\u0639\u0644\u0645 \u0643\u064a\u0641\u064a\u0629 \u062a\u0631\u062a\u064a\u0628 \u0627\u0644\u0639\u0646\u0627\u0635\u0631 \u0628\u0627\u0633\u062a\u062e\u062f\u0627\u0645 Flexbox \u0641\u064a Dhad",
    starterCode: "\u062a\u0646\u0633\u064a\u0642(\"container\", \"display\", \"flex\");\n\u062a\u0646\u0633\u064a\u0642(\"container\", \"justify-content\", \"space-between\");\n\u062a\u0646\u0633\u064a\u0642(\"item\", \"flex\", \"1\");\n\u062a\u0646\u0633\u064a\u0642(\"item\", \"padding\", \"10px\");",
    expectedOutput: "container { display: flex; justify-content: space-between; }\nitem { flex: 1; padding: 10px; }",
    requirements: [
      { type: "must_use_keyword", keyword: "\u062a\u0646\u0633\u064a\u0642" }
    ],
    tier: 8,
    order: 1,
    xpReward: 30,
    points: 30,
    difficulty: "ADVANCED",
    dynamicOutput: "",
    courseId: "WEBDEV"
  },
  {
    title: "\u0627\u0644\u062a\u062e\u0637\u064a\u0637 \u0648\u0627\u0644\u0645\u0631\u0648\u0646\u0629: \u062a\u0645\u0631\u064a\u0646 1",
    description: "\u0627\u0646\u0634\u0626 \u062a\u062e\u0637\u064a\u0637 \u0645\u0631\u0646 \u0628\u0627\u0633\u062a\u062e\u062f\u0627\u0645 Flexbox",
    starterCode: "",
    expectedOutput: "\u062a\u062e\u0637\u064a\u0637 \u0645\u0631\u0646",
    requirements: [
      { type: "must_use_keyword", keyword: "\u062a\u0646\u0633\u064a\u0642" }
    ],
    tier: 8,
    order: 2,
    xpReward: 30,
    points: 30,
    difficulty: "ADVANCED",
    dynamicOutput: "",
    courseId: "WEBDEV"
  },
  {
    title: "\u0627\u0644\u062a\u062e\u0637\u064a\u0637 \u0648\u0627\u0644\u0645\u0631\u0648\u0646\u0629: \u062a\u0645\u0631\u064a\u0646 2",
    description: "\u0635\u0645\u0645 \u0635\u0641\u062d\u0629 \u0628\u062a\u062e\u0637\u064a\u0637 \u0645\u0639\u0642\u062f \u0628\u0627\u0633\u062a\u062e\u062f\u0627\u0645 Flexbox",
    starterCode: "",
    expectedOutput: "\u0635\u0641\u062d\u0629 \u0628\u062a\u062e\u0637\u064a\u0637 \u0645\u0639\u0642\u062f",
    requirements: [
      { type: "must_use_keyword", keyword: "\u062a\u0646\u0633\u064a\u0642" },
      { type: "must_use_keyword", keyword: "\u062c\u062f\u064a\u062f" }
    ],
    tier: 8,
    order: 3,
    xpReward: 30,
    points: 30,
    difficulty: "ADVANCED",
    dynamicOutput: "",
    courseId: "WEBDEV"
  },
  {
    title: "\u0627\u0644\u0646\u0645\u0648\u0630\u062c \u0627\u0644\u062a\u0641\u0627\u0639\u0648\u0644\u064a: \u0627\u0644\u0645\u062b\u0627\u0644",
    description: "\u062a\u0639\u0644\u0645 \u0643\u064a\u0641\u064a\u0629 \u0625\u0646\u0634\u0627\u0621 \u0646\u0645\u0648\u0630\u062c \u0625\u062f\u062e\u0627\u0644 \u0648\u0623\u0632\u0631\u0631 \u0641\u064a Dhad",
    starterCode: "\u0639\u0646\u0627\u0635\u0631(\"form\", [\n  \u0639\u0646\u0627\u0635\u0631(\"input\", \"\", \u0633\u0645\u0629(\"type\", \"text\"), \u0633\u0645\u0629(\"placeholder\", \"\u0627\u0644\u0627\u0633\u0645\")),\n  \u0639\u0646\u0627\u0635\u0631(\"input\", \"\", \u0633\u0645\u0629(\"type\", \"email\"), \u0633\u0645\u0629(\"placeholder\", \"\u0627\u0644\u0628\u0631\u064a\u062f\")),\n  \u0639\u0646\u0627\u0635\u0631(\"button\", \"\u0625\u0631\u0633\u0627\u0644\", \u0633\u0645\u0629(\"type\", \"submit\"))\n]);",
    expectedOutput: "<form>\n  <input type=\"text\" placeholder=\"\u0627\u0644\u0627\u0633\u0645\">\n  <input type=\"email\" placeholder=\"\u0627\u0644\u0628\u0631\u064a\u062f\">\n  <button type=\"submit\">\u0625\u0631\u0633\u0627\u0644</button>\n</form>",
    requirements: [
      { type: "must_use_keyword", keyword: "\u0639\u0646\u0627\u0635\u0631" }
    ],
    tier: 9,
    order: 1,
    xpReward: 30,
    points: 30,
    difficulty: "ADVANCED",
    dynamicOutput: "",
    courseId: "WEBDEV"
  },
  {
    title: "\u0627\u0644\u0646\u0645\u0648\u0630\u062c \u0627\u0644\u062a\u0641\u0627\u0639\u0648\u0644\u064a: \u062a\u0645\u0631\u064a\u0646 1",
    description: "\u0627\u0646\u0634\u0626 \u0646\u0645\u0648\u0630\u062c \u0625\u062f\u062e\u0627\u0644 \u0628\u064a\u0627\u0646\u0627\u062a \u0628\u0633\u064a\u0637",
    starterCode: "",
    expectedOutput: "\u0646\u0645\u0648\u0630\u062c \u0625\u062f\u062e\u0627\u0644",
    requirements: [
      { type: "must_use_keyword", keyword: "\u0639\u0646\u0627\u0635\u0631" }
    ],
    tier: 9,
    order: 2,
    xpReward: 30,
    points: 30,
    difficulty: "ADVANCED",
    dynamicOutput: "",
    courseId: "WEBDEV"
  },
  {
    title: "\u0627\u0644\u0646\u0645\u0648\u0630\u062c \u0627\u0644\u062a\u0641\u0627\u0639\u0648\u0644\u064a: \u062a\u0645\u0631\u064a\u0646 2",
    description: "\u0635\u0645\u0645 \u0646\u0645\u0648\u0630\u062c \u062a\u0633\u062c\u064a\u0644 \u0645\u0633\u062a\u062e\u062f\u0645 \u0643\u0627\u0645\u0644",
    starterCode: "",
    expectedOutput: "\u0646\u0645\u0648\u0630\u062c \u062a\u0633\u062c\u064a\u0644 \u0645\u0633\u062a\u062e\u062f\u0645",
    requirements: [
      { type: "must_use_keyword", keyword: "\u0639\u0646\u0627\u0635\u0631" },
      { type: "must_use_keyword", keyword: "\u062c\u062f\u064a\u062f" }
    ],
    tier: 9,
    order: 3,
    xpReward: 30,
    points: 30,
    difficulty: "ADVANCED",
    dynamicOutput: "",
    courseId: "WEBDEV"
  },
  {
    title: "\u0635\u0641\u062d\u0629 \u0648\u064a\u0628 \u0643\u0627\u0645\u0644\u0629: \u0627\u0644\u0645\u062b\u0627\u0644",
    description: "\u0623\u0646\u0634\u0626 \u0635\u0641\u062d\u0629 \u0648\u064a\u0628 \u0643\u0627\u0645\u0644\u0629 \u062a\u062c\u0645\u0639 \u062c\u0645\u064a\u0639 \u0645\u0627 \u062a\u0639\u0644\u0645\u062a\u0647 \u0628\u0627\u0633\u062a\u062e\u062f\u0627\u0645 Dhad",
    starterCode: "\u0635\u0641\u062d\u0629(\"\u0635\u0641\u062d\u0629 \u0648\u064a\u0628 \u0643\u0627\u0645\u0644\u0629\", [\n  \u0639\u0646\u0627\u0635\u0631(\"h1\", \"\u0645\u0631\u062d\u0628\u0627\u0646 \u0628\u0643\u0645\"),\n  \u0639\u0646\u0627\u0635\u0631(\"p\", \"\u0647\u0630\u0647 \u0635\u0641\u062d\u0629 \u0648\u064a\u0628 \u0645\u062a\u0643\u0627\u0645\u0644\u0629\"),\n  \u0639\u0646\u0627\u0635\u0631(\"ul\", [\u0639\u0646\u0627\u0635\u0631(\"li\", \"HTML\"), \u0639\u0646\u0627\u0635\u0631(\"li\", \"CSS\"), \u0639\u0646\u0627\u0635\u0631(\"li\", \"JavaScript\")]),\n  \u062a\u0646\u0633\u064a\u0642(\"h1\", \"color\", \"navy\")\n]);",
    expectedOutput: "<!DOCTYPE html>\n<html>\n<head><title>\u0635\u0641\u062d\u0629 \u0648\u064a\u0628 \u0643\u0627\u0645\u0644\u0629</title></head>\n<body>\n  <h1>\u0645\u0631\u062d\u0628\u0627\u0646 \u0628\u0643\u0645</h1>\n  <p>\u0647\u0630\u0647 \u0635\u0641\u062d\u0629 \u0648\u064a\u0628 \u0645\u062a\u0643\u0627\u0645\u0644\u0629</p>\n  <ul><li>HTML</li><li>CSS</li><li>JavaScript</li></ul>\n  <style>h1 { color: navy; }</style>\n</body>\n</html>",
    requirements: [
      { type: "must_use_keyword", keyword: "\u0635\u0641\u062d\u0629" },
      { type: "must_use_keyword", keyword: "\u0639\u0646\u0627\u0635\u0631" },
      { type: "must_use_keyword", keyword: "\u062a\u0646\u0633\u064a\u0642" }
    ],
    tier: 10,
    order: 1,
    xpReward: 30,
    points: 30,
    difficulty: "ADVANCED",
    dynamicOutput: "",
    courseId: "WEBDEV"
  },
  {
    title: "\u0635\u0641\u062d\u0629 \u0648\u064a\u0628 \u0643\u0627\u0645\u0644\u0629: \u062a\u0645\u0631\u064a\u0646 1",
    description: "\u0627\u0646\u0634\u0626 \u0635\u0641\u062d\u0629 \u0648\u064a\u0628 \u0645\u062a\u0643\u0627\u0645\u0644\u0629 \u0645\u0646 \u0627\u0644\u0635\u0641\u0631",
    starterCode: "",
    expectedOutput: "\u0635\u0641\u062d\u0629 \u0648\u064a\u0628 \u0645\u062a\u0643\u0627\u0645\u0644\u0629",
    requirements: [
      { type: "must_use_keyword", keyword: "\u0635\u0641\u062d\u0629" },
      { type: "must_use_keyword", keyword: "\u0639\u0646\u0627\u0635\u0631" },
      { type: "must_use_keyword", keyword: "\u062a\u0646\u0633\u064a\u0642" }
    ],
    tier: 10,
    order: 2,
    xpReward: 30,
    points: 30,
    difficulty: "ADVANCED",
    dynamicOutput: "",
    courseId: "WEBDEV"
  },
  {
    title: "\u0635\u0641\u062d\u0629 \u0648\u064a\u0628 \u0643\u0627\u0645\u0644\u0629: \u062a\u0645\u0631\u064a\u0646 2",
    description: "\u0635\u0645\u0645 \u0635\u0641\u062d\u0629 \u0648\u064a\u0628 \u0634\u0627\u0645\u0644\u0629 \u062a\u062c\u0645\u0639 \u0627\u0644\u062a\u0646\u0633\u064a\u0642 \u0648\u0627\u0644\u0646\u0635\u0648\u0635 \u0648\u0627\u0644\u0646\u0645\u0648\u0630\u062c",
    starterCode: "",
    expectedOutput: "\u0635\u0641\u062d\u0629 \u0648\u064a\u0628 \u0634\u0627\u0645\u0644\u0629",
    requirements: [
      { type: "must_use_keyword", keyword: "\u0635\u0641\u062d\u0629" },
      { type: "must_use_keyword", keyword: "\u0639\u0646\u0627\u0635\u0631" },
      { type: "must_use_keyword", keyword: "\u062a\u0646\u0633\u064a\u0642" },
      { type: "must_use_keyword", keyword: "\u062c\u062f\u064a\u062f" }
    ],
    tier: 10,
    order: 3,
    xpReward: 30,
    points: 30,
    difficulty: "ADVANCED",
    dynamicOutput: "",
    courseId: "WEBDEV"
  }
];

module.exports = challenges;
