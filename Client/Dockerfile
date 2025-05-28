# Fase di build
FROM gradle:6.9.4-jdk11 AS build
WORKDIR /app
COPY . .
RUN gradle shadowJar

# Fase di runtime
FROM openjdk:11-jre-slim
WORKDIR /app
COPY --from=build /app/build/libs/*.skill /app/skill.skill

# Avvia l'SDK Furhat e la skill
CMD ["java", "-cp", "skill.skill", "furhatos.skills.Skill"]

