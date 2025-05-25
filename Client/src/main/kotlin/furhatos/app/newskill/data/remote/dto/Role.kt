package furhatos.app.newskill.data.remote.dto

import kotlinx.serialization.Serializable

@Serializable
enum class Role(
    val value: String,
) {
    user,
    model,
}
