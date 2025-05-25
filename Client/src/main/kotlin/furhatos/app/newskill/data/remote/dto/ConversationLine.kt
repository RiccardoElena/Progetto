package furhatos.app.newskill.data.remote.dto

import kotlinx.serialization.Serializable

@Serializable
data class ConversationLine(
    val role: Role,
    val parts: List<Messages>,
)
