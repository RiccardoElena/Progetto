package furhatos.app.newskill.model

import java.util.UUID

data class User(
    val id: UUID,
    val userName: String,
    val securityQuestion: String,
    val personalityVector: PersonalityVector = PersonalityVector.median,
)
