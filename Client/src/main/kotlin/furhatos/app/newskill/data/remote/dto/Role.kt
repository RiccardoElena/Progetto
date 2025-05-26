package furhatos.app.newskill.data.remote.dto

import kotlinx.serialization.Serializable

@Serializable
enum class Role {
    @Suppress("ktlint:standard:enum-entry-name-case")
    user,

    @Suppress("ktlint:standard:enum-entry-name-case")
    model,
}
