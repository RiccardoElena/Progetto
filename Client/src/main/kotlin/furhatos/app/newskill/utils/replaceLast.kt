package furhatos.app.newskill.utils

fun String.replaceLast(
    str: String,
    rep: String,
): String {
    val index = lastIndexOf(str)
    return if (index == -1) this else this.replaceRange(index until (index + str.length), rep)
}
