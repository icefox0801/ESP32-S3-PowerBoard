# CLAUDE.md - Development Guidelines

## Commit Message Convention

Your task is to help the user to generate a commit message and commit the changes using git.

### Guidelines

- DO NOT add any ads such as "Generated with [Claude Code](https://claude.ai/code)"
- Only generate the message for staged files/changes
- Don't add any files using `git add`. The user will decide what to add. 
- Follow the rules below for the commit message.

### Format

```
<type>:<space><message title>

<bullet points summarizing what was updated>
```

### Example Titles

```
feat(auth): add JWT login flow
fix(ui): handle null pointer in sidebar
refactor(api): split user controller logic
docs(readme): add usage section
```

### Example with Title and Body

```
feat(auth): add JWT login flow

- Implemented JWT token validation logic
- Added documentation for the validation component
```

### Rules

* title is lowercase, no period at the end.
* Title should be a clear summary, max 50 characters.
* Use the body (optional) to explain *why*, not just *what*.
* Bullet points should be concise and high-level.

### Avoid

* Vague titles like: "update", "fix stuff"
* Overly long or unfocused titles
* Excessive detail in bullet points

### Allowed Types

| Type     | Description                           |
| -------- | ------------------------------------- |
| feat     | New feature                           |
| fix      | Bug fix                               |
| chore    | Maintenance (e.g., tooling, deps)     |
| docs     | Documentation changes                 |
| refactor | Code restructure (no behavior change) |
| test     | Adding or refactoring tests           |
| style    | Code formatting (no logic change)     |
| perf     | Performance improvements              |

## Development Workflow

### Setup
```bash
git clone <repository>
cd PowerBoard
```

### Build & Upload
```bash
pio run --target upload --upload-port COM3
```

### Monitor
```bash
pio device monitor --port COM3 --baud 115200
```

### Pre-commit Checklist
- [ ] Code compiles without errors
- [ ] Serial output tested on hardware
- [ ] PSRAM functionality verified (if applicable)
- [ ] Commit message follows convention
- [ ] Documentation updated (if needed)

## Project Structure

```
PowerBoard/
├── src/
│   └── main.cpp          # Main application code
├── include/              # Header files
├── lib/                  # Local libraries
├── test/                 # Test files
├── platformio.ini        # PlatformIO configuration
├── .gitignore           # Git ignore rules
└── CLAUDE.md            # This file
```

## Hardware Configuration
- **Board**: ESP32-S3 DevKit
- **PSRAM**: Enabled (8MB)
- **USB**: CDC on boot enabled
- **Serial**: Dual output (USB + UART)
- **Port**: COM3 @ 115200 baud
